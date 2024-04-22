## SEGV in SilentPprf.cpp


### Description

SEGV MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1068:59 in osuCrypto::SilentMultiPprfReceiver::expand

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
SOCKET_DATA_PATH="../Vulnerabilities-in-MPC-Framework/MP-SPDZ/SEGV-SlientPprf-4" LD_PRELOAD="../Vulnerabilities-in-MPC-Framework/librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```

### ASAN

```
AddressSanitizer:DEADLYSIGNAL
=================================================================
==2495730==ERROR: AddressSanitizer: SEGV on unknown address 0x518800023dc0 (pc 0x55a03827f7dd bp 0x0000e0000003 sp 0x7fd6cf211e30 T71)
==2495730==The signal is caused by a READ memory access.
    #0 0x55a03827f7dd in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long)::$_0::operator()(unsigned long) const /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1068:59
    #1 0x55a03827edfb in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1199:9
    #2 0x55a03826f7e9 in osuCrypto::SoftSpokenOT::SmallFieldVoleReceiver::SmallFieldVoleReceiver(unsigned long, unsigned long, osuCrypto::Channel&, osuCrypto::PRNG&, gsl::span<osuCrypto::block const, -1l>, osuCrypto::BitVector, unsigned long, bool) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Vole/SoftSpokenOT/SmallFieldVole.cpp:386:7
    #3 0x55a03826b815 in osuCrypto::SoftSpokenOT::DotSemiHonestSenderWithVole<osuCrypto::SoftSpokenOT::SubspaceVoleMaliciousReceiver<osuCrypto::RepetitionCode>>::setBaseOts(gsl::span<osuCrypto::block, -1l>, osuCrypto::BitVector const&, osuCrypto::PRNG&, osuCrypto::Channel&, bool) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/TwoChooseOne/SoftSpokenOT/DotSemiHonest.cpp:30:3
    #4 0x55a03820e3de in osuCrypto::SoftSpokenOT::DotMaliciousLeakySender::setBaseOts(gsl::span<osuCrypto::block, -1l>, osuCrypto::BitVector const&, osuCrypto::PRNG&, osuCrypto::Channel&) /home/lgp/MP-SPDZ/./local/include/libOTe/TwoChooseOne/SoftSpokenOT/DotMaliciousLeaky.h:75:9
    #5 0x55a03820e3de in OTExtensionWithMatrix::soft_sender(unsigned long) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:152:12
    #6 0x55a03820da75 in OTExtensionWithMatrix::extend(int, BitVector const&) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:128:9
    #7 0x55a0381ca33d in OTMultiplier<GC::TinierSecret<gf2n_mac_key>>::multiply() /home/lgp/MP-SPDZ/./OT/OTMultiplier.hpp:94:13
    #8 0x55a0381c4cc1 in void* run_ot_thread<GC::TinierSecret<gf2n_mac_key>>(void*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:28:31
    #9 0x55a0380f3206 in asan_thread_start(void*) asan_interceptors.cpp.o
    #10 0x7fd72d26b608 in start_thread /build/glibc-e2p3jK/glibc-2.31/nptl/pthread_create.c:477:8
    #11 0x7fd72d147352 in __clone /build/glibc-e2p3jK/glibc-2.31/misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:95

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: SEGV /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1068:59 in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long)::$_0::operator()(unsigned long) const
Thread T71 created by T70 here:
    #0 0x55a0380db1b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x55a0381c1de2 in OTTripleGenerator<GC::TinierSecret<gf2n_mac_key>>::OTTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, gf2n_mac_key, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:135:9
    #2 0x55a03816b961 in NPartyTripleGenerator<GC::TinierSecret<gf2n_mac_key>>::NPartyTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, gf2n_mac_key, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:40:9
    #3 0x55a03816b961 in GC::TinierSharePrep<GC::TinierShare<gf2n_mac_key>>::set_protocol(Beaver<GC::TinierShare<gf2n_mac_key>>&) /home/lgp/MP-SPDZ/./GC/TinierSharePrep.hpp:51:28
    #4 0x55a03815eea3 in SubProcessor<GC::TinierShare<gf2n_mac_key>>::SubProcessor(MAC_Check_<GC::TinierShare<gf2n_mac_key>>&, Preprocessing<GC::TinierShare<gf2n_mac_key>>&, Player&, ArithmeticProcessor*) /home/lgp/MP-SPDZ/./Processor/Processor.hpp:34:9

Thread T70 created by T0 here:
    #0 0x55a0380db1b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x55a03894a4cd in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::prepare(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:203:7
    #2 0x55a0389529b6 in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::run(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:452:3
    #3 0x55a038140c4c in int OnlineMachine::run<Spdz2kShare<64, 64>, Share<gf2n_long>>() /home/lgp/MP-SPDZ/./Processor/OnlineMachine.hpp:181:70
    #4 0x55a038137f4f in main /home/lgp/MP-SPDZ/Machines/spdz2k-party.cpp:45:5
    #5 0x7fd72d04c082 in __libc_start_main /build/glibc-e2p3jK/glibc-2.31/csu/../csu/libc-start.c:308:16

==2495730==ABORTING
```