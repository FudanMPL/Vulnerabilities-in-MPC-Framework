#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/stat.h>
#include <pthread.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>
#include <memory>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <openssl/bn.h>
#include <unistd.h>
#include <mutex>
#include <stdarg.h>


static FILE *(*real_fopen)(const char *pathname, const char *mode) = NULL;

extern "C" {
int BN_rand_range(BIGNUM *rnd, const BIGNUM *range){
    BN_dec2bn(&rnd, "0");
    // std::cout << "new BN_rand_range used" << std::endl;
}
}

std::mutex rand_mtx; 

extern "C" { // without extern C, randombytes_buf will not cover the implementation in the test program
void randombytes_buf(void * const buf, const size_t size){
    #ifdef DEBUG
    cout << "use new randombytes_buf" << endl;
    #endif
    std::lock_guard<std::mutex> lock(rand_mtx);
    std::srand(0);
    for (int i = 0; i < size; i++)
      ((char *) buf)[i] = std::rand() % 256;
}
}



extern "C" {
static int (*real_open)(const char *, int, ...) = nullptr;
int open(const char *pathname, int flags, ...) {
  std::lock_guard<std::mutex> lock(rand_mtx);
  if (real_open == nullptr) {
    real_open = (int(*)(const char *, int, ...))dlsym(RTLD_NEXT, "open");
    if (real_open == nullptr) {
      std::cerr << "Error in dlsym: " << dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  va_list args;
  va_start(args, flags);

  mode_t mode = 0;
  if (flags & O_CREAT) {
    mode = va_arg(args, mode_t);
  }
  va_end(args);

  if (strcmp(pathname, "/dev/random") == 0 || strcmp(pathname, "/dev/urandom") == 0) {

    if (!std::filesystem::exists("../random")) {
      std::cerr << "../random" << " does not exist\n";
      exit(1);
    }
    if (flags & O_CREAT) {
      return real_open("../random", flags, mode);
    } else {
      return real_open("../random", flags);
    }
  }


  if (flags & O_CREAT) {
    return real_open(pathname, flags, mode);
  } else {
    return real_open(pathname, flags);
  }
}
}

extern "C" {

FILE *fopen(const char *pathname, const char *mode) {
  std::lock_guard<std::mutex> lock(rand_mtx);

  if (real_fopen == nullptr){
    real_fopen = (FILE *(*)(const char *, const char *))dlsym(RTLD_NEXT, "fopen");
  }
  if (strcmp(pathname, "/dev/random") == 0 || strcmp(pathname, "/dev/urandom") == 0) {
    if (!std::filesystem::exists("../random")) {
      std::cerr << "../random" << " does not exist\n";
      exit(1);
    }
      return real_fopen("../random", mode);
  }
  FILE *result = real_fopen(pathname, mode);

  return result;
}
}