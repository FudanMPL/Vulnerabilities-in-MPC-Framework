## stack-buffer-overflow in octetStream.cpp

### Description

stack-buffer-overflow MP-SPDZ/Tools/octetStream.cpp:147:3 in octetStream::get_bytes


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
SOCKET_DATA_PATH="../Vulnerabilities-in-MPC-Framework/MP-SPDZ/stack-buffer-overflow-octetStream" LD_PRELOAD="../Vulnerabilities-in-MPC-Framework/librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```

### ASAN

```
==2479299==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7f65dc207f60 at pc 0x561cdfaf866e bp 0x7f65dc1464e0 sp 0x7f65dc145ca0
WRITE of size 193965026 at 0x7f65dc207f60 thread T70
    #0 0x561cdfaf866d in __asan_memcpy (/home/lgp/MP-SPDZ/spdz2k-party.x+0x34766d) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x7f6630e67397 in octetStream::get_bytes(unsigned char*, unsigned long&) /home/lgp/MP-SPDZ/Tools/octetStream.cpp:147:3
    #2 0x561cdfc0d8e3 in void BaseOT::exec_base<ot_sender, ot_receiver>(bool) /home/lgp/MP-SPDZ/OT/BaseOT.cpp:211:19
    #3 0x561cdfc0357f in OTTripleSetup::setup() /home/lgp/MP-SPDZ/OT/OTTripleSetup.cpp:18:21
    #4 0x561cdfb897d8 in OTTripleSetup::OTTripleSetup(Player&, bool) /home/lgp/MP-SPDZ/./OT/OTTripleSetup.h:72:9
    #5 0x561cdfb88920 in OnDemandOTTripleSetup::get_fresh(Player&) /home/lgp/MP-SPDZ/./OT/OTTripleSetup.h:101:30
    #6 0x561cdfb70876 in BaseMachine::fresh_ot_setup(Player&) /home/lgp/MP-SPDZ/./Processor/BaseMachine.h:102:21
    #7 0x561cdfb70876 in GC::TinierSharePrep<GC::TinierShare<gf2n_mac_key>>::set_protocol(Beaver<GC::TinierShare<gf2n_mac_key>>&) /home/lgp/MP-SPDZ/./GC/TinierSharePrep.hpp:52:13
    #8 0x561cdfb63ea3 in SubProcessor<GC::TinierShare<gf2n_mac_key>>::SubProcessor(MAC_Check_<GC::TinierShare<gf2n_mac_key>>&, Preprocessing<GC::TinierShare<gf2n_mac_key>>&, Player&, ArithmeticProcessor*) /home/lgp/MP-SPDZ/./Processor/Processor.hpp:34:9
    #9 0x561cdfb63943 in GC::CcdPrep<GC::TinierSecret<gf2n_mac_key>>::set_protocol(GC::VectorProtocol<GC::TinierSecret<gf2n_mac_key>>&) /home/lgp/MP-SPDZ/./GC/CcdPrep.hpp:36:21
    #10 0x561cdfd5ed8a in GC::ShareThread<GC::TinierSecret<gf2n_mac_key>>::pre_run(Player&, gf2n_mac_key) /home/lgp/MP-SPDZ/./GC/ShareThread.hpp:65:11
    #11 0x561ce0364313 in GC::ShareThread<GC::TinierSecret<gf2n_mac_key>>::ShareThread(Preprocessing<GC::TinierSecret<gf2n_mac_key>>&, Player&, gf2n_mac_key) /home/lgp/MP-SPDZ/./GC/ShareThread.hpp:44:5
    #12 0x561ce0364313 in Processor<Spdz2kShare<64, 64>, Share<gf2n_long>>::Processor(int, Player&, MAC_Check_<Share<gf2n_long>>&, MAC_Check_Z2k<Z2<128>, Z2<64>, Z2<128>, Spdz2kShare<64, 64>>&, Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>&, Program const&) /home/lgp/MP-SPDZ/./Processor/Processor.hpp:88:3
    #13 0x561ce035eb23 in thread_info<Spdz2kShare<64, 64>, Share<gf2n_long>>::Sub_Main_Func() /home/lgp/MP-SPDZ/./Processor/Online-Thread.hpp:102:19
    #14 0x561ce0350638 in thread_info<Spdz2kShare<64, 64>, Share<gf2n_long>>::Main_Func(void*) /home/lgp/MP-SPDZ/./Processor/Online-Thread.hpp:374:8
    #15 0x561cdfaf8206 in asan_thread_start(void*) asan_interceptors.cpp.o
    #16 0x7f6630393608 in start_thread /build/glibc-e2p3jK/glibc-2.31/nptl/pthread_create.c:477:8
    #17 0x7f663026f352 in __clone /build/glibc-e2p3jK/glibc-2.31/misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:95

Address 0x7f65dc207f60 is located in stack of thread T70 at offset 792992 in frame
    #0 0x561cdfc0c56f in void BaseOT::exec_base<ot_sender, ot_receiver>(bool) /home/lgp/MP-SPDZ/OT/BaseOT.cpp:122

  This frame has 13 object(s):
    [32, 1632) 'sender' (line 128)
    [1760, 791424) 'receiver' (line 129)
    [791680, 791712) 'ref.tmp.i'
    [791744, 791745) 'ref.tmp2.i'
    [791760, 791768) 'len' (line 124)
    [791792, 792464) 'G' (line 125)
    [792592, 792616) 'os' (line 127)
    [792656, 792657) 'ref.tmp' (line 127)
    [792672, 792704) 'S_pack' (line 131)
    [792736, 792992) 'Rs_pack' (line 132)
    [793056, 793312) 'sender_keys' (line 133) <== Memory access at offset 792992 partially underflows this variable
    [793376, 793504) 'receiver_keys' (line 134) <== Memory access at offset 792992 partially underflows this variable
    [793536, 793540) 'cs' (line 135) <== Memory access at offset 792992 partially underflows this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork
      (longjmp and C++ exceptions *are* supported)
Thread T70 created by T0 here:
    #0 0x561cdfae01b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x561ce034f4cd in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::prepare(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:203:7
    #2 0x561ce03579b6 in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::run(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:452:3
    #3 0x561cdfb45c4c in int OnlineMachine::run<Spdz2kShare<64, 64>, Share<gf2n_long>>() /home/lgp/MP-SPDZ/./Processor/OnlineMachine.hpp:181:70
    #4 0x561cdfb3cf4f in main /home/lgp/MP-SPDZ/Machines/spdz2k-party.cpp:45:5
    #5 0x7f6630174082 in __libc_start_main /build/glibc-e2p3jK/glibc-2.31/csu/../csu/libc-start.c:308:16

SUMMARY: AddressSanitizer: stack-buffer-overflow (/home/lgp/MP-SPDZ/spdz2k-party.x+0x34766d) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3) in __asan_memcpy
Shadow bytes around the buggy address:
  0x7f65dc207c80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f65dc207d00: 00 00 00 00 00 00 00 00 00 00 f2 f2 f2 f2 f2 f2
  0x7f65dc207d80: f2 f2 f2 f2 f2 f2 f2 f2 f2 f2 00 00 00 f2 f2 f2
  0x7f65dc207e00: f2 f2 f8 f2 00 00 00 00 f2 f2 f2 f2 00 00 00 00
  0x7f65dc207e80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x7f65dc207f00: 00 00 00 00 00 00 00 00 00 00 00 00[f2]f2 f2 f2
  0x7f65dc207f80: f2 f2 f2 f2 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f65dc208000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x7f65dc208080: 00 00 00 00 f2 f2 f2 f2 f2 f2 f2 f2 00 00 00 00
  0x7f65dc208100: 00 00 00 00 00 00 00 00 00 00 00 00 f2 f2 f2 f2
  0x7f65dc208180: 04 f3 f3 f3 00 00 00 00 00 00 00 00 00 00 00 00
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
==2479299==ABORTING
```