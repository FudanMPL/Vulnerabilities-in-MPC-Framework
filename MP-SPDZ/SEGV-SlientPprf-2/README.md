## SEGV in SilentPprf.cpp


### Description

SEGV MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1083:47 in osuCrypto::SilentMultiPprfReceiver::expand


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
SOCKET_DATA_PATH="../Vulnerabilities-in-MPC-Framework/MP-SPDZ/SEGV-SlientPprf-2" LD_PRELOAD="../Vulnerabilities-in-MPC-Framework/librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```

### ASAN

```
AddressSanitizer:DEADLYSIGNAL
=================================================================
==2418663==ERROR: AddressSanitizer: SEGV on unknown address 0x7b100007c380 (pc 0x55934620c808 bp 0x1fffffffffffffff sp 0x7fa40e5cbcf0 T83)
==2418663==The signal is caused by a READ memory access.
    #0 0x55934620c808 in osuCrypto::block::mm_xor_si128(osuCrypto::block const&) const /home/lgp/MP-SPDZ/deps/libOTe/cryptoTools/cryptoTools/../cryptoTools/Common/block.h:133:20
    #1 0x55934620c808 in osuCrypto::block::operator^(osuCrypto::block const&) const /home/lgp/MP-SPDZ/deps/libOTe/cryptoTools/cryptoTools/../cryptoTools/Common/block.h:125:20
    #2 0x55934620c808 in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long)::$_0::operator()(unsigned long) const /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1083:47
    #3 0x55934620bdfb in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1199:9
    #4 0x5593461fc7e9 in osuCrypto::SoftSpokenOT::SmallFieldVoleReceiver::SmallFieldVoleReceiver(unsigned long, unsigned long, osuCrypto::Channel&, osuCrypto::PRNG&, gsl::span<osuCrypto::block const, -1l>, osuCrypto::BitVector, unsigned long, bool) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Vole/SoftSpokenOT/SmallFieldVole.cpp:386:7
    #5 0x5593461f8815 in osuCrypto::SoftSpokenOT::DotSemiHonestSenderWithVole<osuCrypto::SoftSpokenOT::SubspaceVoleMaliciousReceiver<osuCrypto::RepetitionCode>>::setBaseOts(gsl::span<osuCrypto::block, -1l>, osuCrypto::BitVector const&, osuCrypto::PRNG&, osuCrypto::Channel&, bool) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/TwoChooseOne/SoftSpokenOT/DotSemiHonest.cpp:30:3
    #6 0x55934619b3de in osuCrypto::SoftSpokenOT::DotMaliciousLeakySender::setBaseOts(gsl::span<osuCrypto::block, -1l>, osuCrypto::BitVector const&, osuCrypto::PRNG&, osuCrypto::Channel&) /home/lgp/MP-SPDZ/./local/include/libOTe/TwoChooseOne/SoftSpokenOT/DotMaliciousLeaky.h:75:9
    #7 0x55934619b3de in OTExtensionWithMatrix::soft_sender(unsigned long) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:152:12
    #8 0x55934619aa75 in OTExtensionWithMatrix::extend(int, BitVector const&) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:128:9
    #9 0x559346a23557 in OTMultiplier<Spdz2kShare<66, 64>>::multiplyForTriples() /home/lgp/MP-SPDZ/./OT/OTMultiplier.hpp:271:21
    #10 0x559346a131a2 in OTMultiplier<Spdz2kShare<66, 64>>::multiply() /home/lgp/MP-SPDZ/./OT/OTMultiplier.hpp:129:17
    #11 0x559346a10421 in void* run_ot_thread<Spdz2kShare<66, 64>>(void*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:28:31
    #12 0x559346080206 in asan_thread_start(void*) asan_interceptors.cpp.o
    #13 0x7fa4807dc608 in start_thread /build/glibc-e2p3jK/glibc-2.31/nptl/pthread_create.c:477:8
    #14 0x7fa4806b8352 in __clone /build/glibc-e2p3jK/glibc-2.31/misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:95

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: SEGV /home/lgp/MP-SPDZ/deps/libOTe/cryptoTools/cryptoTools/../cryptoTools/Common/block.h:133:20 in osuCrypto::block::mm_xor_si128(osuCrypto::block const&) const
Thread T83 created by T70 here:
    #0 0x5593460681b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x559346a0fa42 in OTTripleGenerator<Spdz2kShare<66, 64>>::OTTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, Z2<64>, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:135:9
    #2 0x5593469ff595 in NPartyTripleGenerator<Spdz2kShare<66, 64>>::NPartyTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, Z2<64>, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:40:9
    #3 0x5593469ff595 in Spdz2kTripleGenerator<Spdz2kShare<66, 64>>::Spdz2kTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, Z2<64>, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:67:9
    #4 0x5593469ff595 in OTPrep<Spdz2kShare<66, 64>>::set_protocol(SPDZ2k<Spdz2kShare<66, 64>>&) /home/lgp/MP-SPDZ/./Protocols/MascotPrep.hpp:44:28
    #5 0x5593469f375b in SubProcessor<Spdz2kShare<66, 64>>::SubProcessor(MAC_Check_Z2k<Z2<130>, Z2<64>, Z2<130>, Spdz2kShare<66, 64>>&, Preprocessing<Spdz2kShare<66, 64>>&, Player&, ArithmeticProcessor*) /home/lgp/MP-SPDZ/./Processor/Processor.hpp:34:9

Thread T70 created by T0 here:
    #0 0x5593460681b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x5593468d74cd in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::prepare(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:203:7
    #2 0x5593468df9b6 in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::run(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:452:3
    #3 0x5593460cdc4c in int OnlineMachine::run<Spdz2kShare<64, 64>, Share<gf2n_long>>() /home/lgp/MP-SPDZ/./Processor/OnlineMachine.hpp:181:70
    #4 0x5593460c4f4f in main /home/lgp/MP-SPDZ/Machines/spdz2k-party.cpp:45:5
    #5 0x7fa4805bd082 in __libc_start_main /build/glibc-e2p3jK/glibc-2.31/csu/../csu/libc-start.c:308:16

==2418663==ABORTING
```