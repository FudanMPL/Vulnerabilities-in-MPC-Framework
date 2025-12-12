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
#include <queue>
#include <mutex>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <mutex>
#include <stdarg.h>
#include <cstdlib>


using namespace std;

#include "random_control.hpp"

#define MAX_SOCKETS 100
#define BUFFER_SIZE 1024000
// #define DEBUG

typedef struct {
  struct sockaddr_in bind_address;
  struct sockaddr_in connect_address;
} MySocket;



int my_number = 0;

std::string dir_path;

std::map<int, MySocket> sockets;

std::map<int, std::unique_ptr<std::ifstream>> recv_files;

std::map<int, std::unique_ptr<std::ifstream>> meta_files;

std::queue<int> socket_fds;

std::queue<int> accept_fds;

std::mutex init_mtx; 


static int (*real_close)(int fd) = NULL;

std::mutex fun_mtx; 







void add_to_map(int number){
  if (sockets.find(number) == sockets.end()){
    MySocket tmp;
    sockets[number] = tmp;
    auto  recv_file = std::make_unique<std::ifstream>(dir_path  + std::to_string(number) +".txt", std::ios::binary);
    recv_files[number] = std::move(recv_file);
  }
}

int initialize_fds() {
  std::lock_guard<std::mutex> lock(init_mtx);
  const char * tmp_path = getenv("SOCKET_DATA_PATH");
  if (tmp_path == nullptr){
    std::cout << "SOCKET_DATA_PATH environment variable is not set " << std::endl;
    exit(1);
  }
  dir_path = tmp_path;
  dir_path = dir_path + "/";

  if (socket_fds.empty()){
    std::ifstream file(dir_path  + "acceptfd.txt");
    int number;
    while(file >> number){
      accept_fds.push(number);
      add_to_map(number);
    }
    file.close();
    std::ifstream file2(dir_path  + "socketfd.txt");
    while(file2 >> number){
      socket_fds.push(number);
      add_to_map(number);
    }
    file2.close();
  }
  return 0;
}


std::mutex socket_mtx; 

int socket(int domain, int type, int protocol) {
  #ifdef DEBUG
  cout << "use new socket" << endl;
  #endif
  if (initialize_fds() == -1){
    return -1;
  }
  std::lock_guard<std::mutex> lock(socket_mtx);
  if (!socket_fds.empty()){
    int fd = socket_fds.front();
    socket_fds.pop();
    #ifdef DEBUG
    cout << "return socket fd = " << fd << endl;
    #endif

    return fd;
  }
  return -1;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
  #ifdef DEBUG
  cout << "use new accept sockfd = " << sockfd  << endl;
  #endif

  std::lock_guard<std::mutex> lock(socket_mtx);
  if (!accept_fds.empty()){
    int fd = accept_fds.front();
    #ifdef DEBUG
    cout << "return accept_fd = " << fd << endl;
    #endif

    accept_fds.pop();
    return fd;
  }
  return -1;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
  #ifdef DEBUG
  cout << "use new bind sockfd = " << sockfd << endl;
  #endif

  if (sockfd < 0 || sockfd >= MAX_SOCKETS) return -1;
  memcpy(&sockets[sockfd].bind_address, addr, sizeof(struct sockaddr_in));
  return 0;
}

int listen(int sockfd, int backlog){
  #ifdef DEBUG
  cout << "use new listen sockfd = " << sockfd << endl;
  #endif

  if (sockfd < 0 || sockfd >= MAX_SOCKETS) return -1;
  return 0;
}




int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
  #ifdef DEBUG
  cout << "use new connect sockfd = " << sockfd << endl;
  #endif

  memcpy(&sockets[sockfd].connect_address, addr, sizeof(struct sockaddr_in));
  return 0;
}

int close(int sockfd){
  #ifdef DEBUG
  cout << "use new close sockfd = " << sockfd << endl;
  #endif

  return 0;
}



ssize_t recv(int sockfd, void * buf, size_t n, int flag){
  #ifdef DEBUG
  cout << "use new recv sockfd = " << sockfd << " n = " << n << " flag = " << flag << endl;
  #endif


  recv_files[sockfd]->read((char *)buf, n);

  
  return n;
}

ssize_t send(int sockfd, const void * buf, size_t n, int flag){
  #ifdef DEBUG
  cout << "use new send sockfd = " << sockfd << " n = "  << n << endl; 
  #endif

  return n;
}

int setsockopt(int sockfd, int level, int optname,
               const void* optval, socklen_t optlen){
  #ifdef DEBUG
  cout << "use new setsockopt" << endl;
  #endif              

  return 0;
}

int getpeername (int __fd, __SOCKADDR_ARG __addr,
			socklen_t *__restrict __len){
  #ifdef DEBUG
  cout << "use new getpeername fd = " << __fd << endl;      
  #endif   

  memcpy(__addr, &sockets[__fd].connect_address, sizeof(struct sockaddr_in));
  *__len = sizeof(struct sockaddr_in);
  return 0;
}

int getsockname (int __fd, __SOCKADDR_ARG __addr,
			socklen_t *__restrict __len){
  #ifdef DEBUG
  cout << "use new getsockname fd = " << __fd << endl;       
  #endif   

  memcpy(__addr, &sockets[__fd].bind_address, sizeof(struct sockaddr_in));
  *__len = sizeof(struct sockaddr_in);
  return 0;
}

