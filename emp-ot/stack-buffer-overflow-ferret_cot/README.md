## SEGV in ferret_cot.hpp

### CVE ID

CVE-2024-31803

### Affected Product

emp-ot

### Affected Version

release 0.2.4

commit: eb0daf2a7a88c44b419f6d1276dc19e66f80776f


### Vulnerability Type

Buffer Overflow


### Description

Buffer Overflow vulnerability in emp-ot v.0.2.4 allows a remote attacker to execute arbitrary code via the FerretCOT<T>::read_pre_data128_from_file function.

emp-ot/emp-ot/ferret/ferret_cot.hpp:199 emp::FerretCOTemp::NetIO::read_pre_data128_from_file



### Replay

1. Download the source code of emp-ot and change the working directory to emp-ot/ 

2. Add the following configuration in CMakeList.txt

```
set(CMAKE_CXX_FLAGS` "${CMAKE_CXX_FLAGS} -lrt -fsanitize=address -g -fprofile-arcs `-ftest-coverage")
```

3. Recompile

```
make clean
cmake .
make
```

4. Run the test in the emp-ot directory

```
./run ./bin/test_ot logn
```



### ASAN

```
==94168==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffe1879bc14 at pc 0x7f741bcc97cf bp 0x7ffe1879bb50 sp 0x7ffe1879b2f8
==94164==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffcaefaca64 at pc 0x7fdd438a87cf bp 0x7ffcaefac9a0 sp 0x7ffcaefac148
WRITE of size 8 at 0x7ffe1879bc14 thread T0
WRITE of size 8 at 0x7ffcaefaca64 thread T0
    #0 0x7fdd438a87ce in __interceptor_fread ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:1046
    #0 0x7f741bcc97ce in __interceptor_fread ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:1046
    #1 0x564e0ec8f6f7 in fread /usr/include/x86_64-linux-gnu/bits/stdio2.h:297
    #2 0x564e0ec8f6f7 in emp::FileIO::recv_data_internal(void*, int) /usr/local/include/emp-tool/io/file_io_channel.h:50
    #3 0x564e0ecbbc34 in emp::IOChannel<emp::FileIO>::recv_data(void*, unsigned long) /usr/local/include/emp-tool/io/io_channel.h:19
    #4 0x564e0ecbbc34 in emp::FerretCOT<emp::NetIO>::read_pre_data128_from_file(void*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:199
    #5 0x564e0ece908f in emp::FerretCOT<emp::NetIO>::setup(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:114
    #6 0x564e0eced696 in emp::FerretCOT<emp::NetIO>::FerretCOT(int, int, emp::NetIO**, bool, bool, emp::PrimalLPNParameter, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:24
    #7 0x564e0ec79cfd in main /home/zw/emp-ot/test/ot.cpp:33
    #8 0x7fdd42d33082 in __libc_start_main ../csu/libc-start.c:308
    #9 0x564e0ec7b07d in _start (/home/zw/emp-ot/bin/test_ot+0x3607d)

Address 0x7ffcaefaca64 is located in stack of thread T0 at offset 36 in frame
    #0 0x564e0ecbb9bf in emp::FerretCOT<emp::NetIO>::read_pre_data128_from_file(void*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:196

  This frame has 6 object(s):
    [32, 36) 'in_party' (line 198) <== Memory access at offset 36 overflows this variable
    [48, 56) 'nin' (line 203)
    [80, 88) 'tin' (line 203)
    [112, 120) 'kin' (line 203)
    [144, 160) 'delta' (line 201)
    [176, 216) 'fio' (line 197)
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-buffer-overflow ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:1046 in __interceptor_fread
Shadow bytes around the buggy address:
  0x100015ded8f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015ded900: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015ded910: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015ded920: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015ded930: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x100015ded940: 00 00 00 00 00 00 00 00 f1 f1 f1 f1[04]f2 00 f2
  0x100015ded950: f2 f2 00 f2 f2 f2 00 f2 f2 f2 00 00 f2 f2 00 00
  0x100015ded960: 00 00 00 f3 f3 f3 f3 f3 00 00 00 00 00 00 00 00
  0x100015ded970: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015ded980: 00 00 00 00 00 00 00 00 f1 f1 f1 f1 01 f2 01 f2
  0x100015ded990: f8 f2 f8 f2 f8 f2 f2 f2 f8 f2 f2 f2 00 00 f2 f2
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==94164==ABORTING
    #1 0x55fbc1f376f7 in fread /usr/include/x86_64-linux-gnu/bits/stdio2.h:297
    #2 0x55fbc1f376f7 in emp::FileIO::recv_data_internal(void*, int) /usr/local/include/emp-tool/io/file_io_channel.h:50
    #3 0x55fbc1f63c34 in emp::IOChannel<emp::FileIO>::recv_data(void*, unsigned long) /usr/local/include/emp-tool/io/io_channel.h:19
    #4 0x55fbc1f63c34 in emp::FerretCOT<emp::NetIO>::read_pre_data128_from_file(void*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:199
    #5 0x55fbc1f9108f in emp::FerretCOT<emp::NetIO>::setup(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:114
    #6 0x55fbc1f943aa in emp::FerretCOT<emp::NetIO>::setup(long long __vector(2), std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:86
    #7 0x55fbc1f95b42 in emp::FerretCOT<emp::NetIO>::FerretCOT(int, int, emp::NetIO**, bool, bool, emp::PrimalLPNParameter, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:23
    #8 0x55fbc1f21cfd in main /home/zw/emp-ot/test/ot.cpp:33
    #9 0x7f741b154082 in __libc_start_main ../csu/libc-start.c:308
    #10 0x55fbc1f2307d in _start (/home/zw/emp-ot/bin/test_ot+0x3607d)

Address 0x7ffe1879bc14 is located in stack of thread T0 at offset 36 in frame
    #0 0x55fbc1f639bf in emp::FerretCOT<emp::NetIO>::read_pre_data128_from_file(void*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >) /home/zw/emp-ot/emp-ot/ferret/ferret_cot.hpp:196

  This frame has 6 object(s):
    [32, 36) 'in_party' (line 198) <== Memory access at offset 36 overflows this variable
    [48, 56) 'nin' (line 203)
    [80, 88) 'tin' (line 203)
    [112, 120) 'kin' (line 203)
    [144, 160) 'delta' (line 201)
    [176, 216) 'fio' (line 197)
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-buffer-overflow ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors.inc:1046 in __interceptor_fread
Shadow bytes around the buggy address:
  0x1000430eb730: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000430eb740: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000430eb750: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000430eb760: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000430eb770: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f1 f1
=>0x1000430eb780: f1 f1[04]f2 00 f2 f2 f2 00 f2 f2 f2 00 f2 f2 f2
  0x1000430eb790: 00 00 f2 f2 00 00 00 00 00 f3 f3 f3 f3 f3 00 00
  0x1000430eb7a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000430eb7b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f1 f1
  0x1000430eb7c0: f1 f1 01 f2 01 f2 f8 f2 f8 f2 f8 f2 f2 f2 f8 f2
  0x1000430eb7d0: f2 f2 00 00 f2 f2 00 00 00 00 00 00 00 00 f2 f2
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==94168==ABORTING
```