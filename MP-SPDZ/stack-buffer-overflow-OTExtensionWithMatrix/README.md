## SEGV in SilentPprf.cpp


### Description

stack-buffer-overflow MP-SPDZ/OT/OTExtensionWithMatrix.cpp:134:5 in OTExtensionWithMatrix::extend


### Version

commit: a4f08e6ca9d0cfd439fbb1bd366edf91a61e9196

### Replay

1. Download the source code of MP-SPDZ and change the working directory to MP-SPDZ/ 

2. Add the following code to Makefile

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
SOCKET_DATA_PATH="../Vulnerabilities-in-MPC-Framework/MP-SPDZ/stack-buffer-overflow-OTExtensionWithMatrix" LD_PRELOAD="../Vulnerabilities-in-MPC-Framework/librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```

### ASAN

```
=================================================================
==2440880==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7f516eded066 at pc 0x5589fd1c9b9b bp 0x7f516feec920 sp 0x7f516feec0e8
READ of size 7 at 0x7f516eded066 thread T71
    #0 0x5589fd1c9b9a in strlen (/home/lgp/MP-SPDZ/spdz2k-party.x+0x2c3b9a) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x7f51ce1fb0ff in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>::compare(char const*) const (/lib/x86_64-linux-gnu/libstdc++.so.6+0x15c0ff) (BuildId: 20422e448604b560d74d2eb3befe56d6655830db)
    #2 0x5589fd367967 in bool std::operator==<char, std::char_traits<char>, std::allocator<char>>(char const*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /usr/lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/basic_string.h:6291:20
    #3 0x5589fd367967 in OTExtensionWithMatrix::extend(int, BitVector const&) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:134:5
    #4 0x5589fd32433d in OTMultiplier<GC::TinierSecret<gf2n_mac_key>>::multiply() /home/lgp/MP-SPDZ/./OT/OTMultiplier.hpp:94:13
    #5 0x5589fd31ecc1 in void* run_ot_thread<GC::TinierSecret<gf2n_mac_key>>(void*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:28:31
    #6 0x5589fd24d206 in asan_thread_start(void*) asan_interceptors.cpp.o
    #7 0x7f51cdf35608 in start_thread /build/glibc-e2p3jK/glibc-2.31/nptl/pthread_create.c:477:8
    #8 0x7f51cde11352 in __clone /build/glibc-e2p3jK/glibc-2.31/misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:95

Address 0x7f516eded066 is located in stack of thread T71 at offset 102 in frame
    #0 0x5589fd3675ef in OTExtensionWithMatrix::extend(int, BitVector const&) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:108

  This frame has 4 object(s):
    [32, 48) 'ref.tmp.i47'
    [64, 80) 'ref.tmp.i'
    [96, 102) 'buf' (line 132) <== Memory access at offset 102 overflows this variable
    [128, 160) 'ref.tmp' (line 134)
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
Thread T71 created by T70 here:
    #0 0x5589fd2351b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x5589fd31bde2 in OTTripleGenerator<GC::TinierSecret<gf2n_mac_key>>::OTTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, gf2n_mac_key, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:135:9
    #2 0x5589fd2c5961 in NPartyTripleGenerator<GC::TinierSecret<gf2n_mac_key>>::NPartyTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, gf2n_mac_key, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:40:9
    #3 0x5589fd2c5961 in GC::TinierSharePrep<GC::TinierShare<gf2n_mac_key>>::set_protocol(Beaver<GC::TinierShare<gf2n_mac_key>>&) /home/lgp/MP-SPDZ/./GC/TinierSharePrep.hpp:51:28
    #4 0x5589fd2b8ea3 in SubProcessor<GC::TinierShare<gf2n_mac_key>>::SubProcessor(MAC_Check_<GC::TinierShare<gf2n_mac_key>>&, Preprocessing<GC::TinierShare<gf2n_mac_key>>&, Player&, ArithmeticProcessor*) /home/lgp/MP-SPDZ/./Processor/Processor.hpp:34:9

Thread T70 created by T0 here:
    #0 0x5589fd2351b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x5589fdaa44cd in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::prepare(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:203:7
    #2 0x5589fdaac9b6 in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::run(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:452:3
    #3 0x5589fd29ac4c in int OnlineMachine::run<Spdz2kShare<64, 64>, Share<gf2n_long>>() /home/lgp/MP-SPDZ/./Processor/OnlineMachine.hpp:181:70
    #4 0x5589fd291f4f in main /home/lgp/MP-SPDZ/Machines/spdz2k-party.cpp:45:5
    #5 0x7f51cdd16082 in __libc_start_main /build/glibc-e2p3jK/glibc-2.31/csu/../csu/libc-start.c:308:16

SUMMARY: AddressSanitizer: stack-buffer-overflow (/home/lgp/MP-SPDZ/spdz2k-party.x+0x2c3b9a) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3) in strlen
Shadow bytes around the buggy address:
  0x7f516edecd80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f516edece00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f516edece80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f516edecf00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f516edecf80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x7f516eded000: f1 f1 f1 f1 f8 f8 f2 f2 f8 f8 f2 f2[06]f2 f2 f2
  0x7f516eded080: 00 00 00 00 f3 f3 f3 f3 00 00 00 00 00 00 00 00
  0x7f516eded100: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x7f516eded180: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x7f516eded200: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
  0x7f516eded280: f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5 f5
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
==2440880==ABORTING
```