## Requested allocation size exceeds maximum supported size


### Description

requested allocation size 0x12a0000000000040 (0x12a0000000001040 after adjustments for alignment, red zones etc.) exceeds maximum supported size of 0x10000000000


### Version

commit: a4f08e6ca9d0cfd439fbb1bd366edf91a61e9196

### Replay

1. Download the source code of MP-SPDZ, change the working directory to MP-SPDZ/, and compile it

```
cd MP-SPDZ/
make -j 8 tldr
```


2. Add the following code to Makefile of MP-SPDZ 

```
CFLAGS +=  -fsanitize=address -g 
```
3. Modify sysRandomSeed in MP-SPDZ/deps/libOTe/cryptoTools/cryptoTools/Common/Defines.cpp as follows:

```
    block sysRandomSeed()
    {
        // std::random_device rd;
        // auto ret = std::array<u32, 4>{rd(), rd(), rd(), rd()};
        // block blk;
        // memcpy(&blk, &ret, sizeof(block));
        // return blk;
        auto ret = std::array<u32, 4>{0, 0, 0, 0};
        block blk;
        memcpy(&blk, &ret, sizeof(block));
        return blk;
    }
```

4. Compile spdz2k-party.x of MP-SPDZ

```
make clean
make -j 8 spdz2k-party.x
```

5. Add the following test.mpc to MP-SPDZ/Programs/Source/ and complie it with compile.py of MP-SPDZ

```
from sorting import radix_sort
a = sint.Array(3)
a[0] = 1
a[1] = 3
a[2] = 2
radix_sort(a, a, 4)
print_ln("a = %s",a.reveal())
b = sint.Array(3)
b[0] = 1
b[1] = 3
b[2] = 2
c = a.get_vector() > b.get_vector()
print_ln("c = %s",c.reveal())
```

```
python3 ./compile.py -R 64 test
```


6. Run the spdz2k-party.x with librecv_hook.so and the socket data inside this directory.


```
SOCKET_DATA_PATH="PATH_TO_THIS_DIRECTORY" LD_PRELOAD="PATH_TO_librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```
For example,
```
SOCKET_DATA_PATH="../Vulnerabilities-in-MPC-Framework/MP-SPDZ/requested-allocation-size" LD_PRELOAD="../Vulnerabilities-in-MPC-Framework/librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```

### ASAN

```
=================================================================
==2461797==ERROR: AddressSanitizer: requested allocation size 0x12a0000000000040 (0x12a0000000001040 after adjustments for alignment, red zones etc.) exceeds maximum supported size of 0x10000000000 (thread T0)
    #0 0x55da7826e3cd in operator new(unsigned long) (/home/lgp/MP-SPDZ/spdz2k-party.x+0x3893cd) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x7f3c9a71e382 in __gnu_cxx::new_allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>::allocate(unsigned long, void const*) /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/ext/new_allocator.h:127:27
    #2 0x7f3c9a71e382 in std::allocator_traits<std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>>::allocate(std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>&, unsigned long) /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/alloc_traits.h:464:20
    #3 0x7f3c9a71e382 in std::_Vector_base<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>>::_M_allocate(unsigned long) /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/stl_vector.h:346:20
    #4 0x7f3c9a71e382 in std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>>>::reserve(unsigned long) /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/vector.tcc:78:22

==2461797==HINT: if you don't care about these errors you may set allocator_may_return_null=1
SUMMARY: AddressSanitizer: allocation-size-too-big (/home/lgp/MP-SPDZ/spdz2k-party.x+0x3893cd) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3) in operator new(unsigned long)
==2461797==ABORTING
```