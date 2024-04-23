## SEGV in SilentPprf.cpp


### Description

SEGV MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:140:41 in osuCrypto::copyOut

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
SOCKET_DATA_PATH="../Vulnerabilities-in-MPC-Framework/MP-SPDZ/SEGV-SlientPprf-3" LD_PRELOAD="../Vulnerabilities-in-MPC-Framework/librecv_hook.so" ./spdz2k-party.x 0 test -pn 10166 -h localhost
```

### ASAN

```
AddressSanitizer:DEADLYSIGNAL
=================================================================
==2455232==ERROR: AddressSanitizer: SEGV on unknown address 0x5210000d7b00 (pc 0x56384a86d836 bp 0x7faf2dd49e48 sp 0x7faf2dd49df0 T80)
==2455232==The signal is caused by a WRITE memory access.
    #0 0x56384a86d836 in osuCrypto::copyOut(gsl::span<std::array<osuCrypto::block, 8ul>, -1l>, osuCrypto::MatrixView<osuCrypto::block>, unsigned long, unsigned long, osuCrypto::PprfOutputFormat) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:140:41
    #1 0x56384a8734b6 in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long)::$_0::operator()(unsigned long) const /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1191:17
    #2 0x56384a871dfb in osuCrypto::SilentMultiPprfReceiver::expand(osuCrypto::Channel&, osuCrypto::PRNG&, osuCrypto::MatrixView<osuCrypto::block>, osuCrypto::PprfOutputFormat, bool, unsigned long) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:1199:9
    #3 0x56384a8627e9 in osuCrypto::SoftSpokenOT::SmallFieldVoleReceiver::SmallFieldVoleReceiver(unsigned long, unsigned long, osuCrypto::Channel&, osuCrypto::PRNG&, gsl::span<osuCrypto::block const, -1l>, osuCrypto::BitVector, unsigned long, bool) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Vole/SoftSpokenOT/SmallFieldVole.cpp:386:7
    #4 0x56384a85e815 in osuCrypto::SoftSpokenOT::DotSemiHonestSenderWithVole<osuCrypto::SoftSpokenOT::SubspaceVoleMaliciousReceiver<osuCrypto::RepetitionCode>>::setBaseOts(gsl::span<osuCrypto::block, -1l>, osuCrypto::BitVector const&, osuCrypto::PRNG&, osuCrypto::Channel&, bool) /home/lgp/MP-SPDZ/deps/libOTe/libOTe/TwoChooseOne/SoftSpokenOT/DotSemiHonest.cpp:30:3
    #5 0x56384a8013de in osuCrypto::SoftSpokenOT::DotMaliciousLeakySender::setBaseOts(gsl::span<osuCrypto::block, -1l>, osuCrypto::BitVector const&, osuCrypto::PRNG&, osuCrypto::Channel&) /home/lgp/MP-SPDZ/./local/include/libOTe/TwoChooseOne/SoftSpokenOT/DotMaliciousLeaky.h:75:9
    #6 0x56384a8013de in OTExtensionWithMatrix::soft_sender(unsigned long) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:152:12
    #7 0x56384a800a75 in OTExtensionWithMatrix::extend(int, BitVector const&) /home/lgp/MP-SPDZ/OT/OTExtensionWithMatrix.cpp:128:9
    #8 0x56384b0128ed in OTMultiplier<Spdz2kShare<64, 64>>::multiply() /home/lgp/MP-SPDZ/./OT/OTMultiplier.hpp:94:13
    #9 0x56384b010511 in void* run_ot_thread<Spdz2kShare<64, 64>>(void*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:28:31
    #10 0x56384a6e6206 in asan_thread_start(void*) asan_interceptors.cpp.o
    #11 0x7faf96a3b608 in start_thread /build/glibc-e2p3jK/glibc-2.31/nptl/pthread_create.c:477:8
    #12 0x7faf96917352 in __clone /build/glibc-e2p3jK/glibc-2.31/misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:95

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: SEGV /home/lgp/MP-SPDZ/deps/libOTe/libOTe/Tools/SilentPprf.cpp:140:41 in osuCrypto::copyOut(gsl::span<std::array<osuCrypto::block, 8ul>, -1l>, osuCrypto::MatrixView<osuCrypto::block>, unsigned long, unsigned long, osuCrypto::PprfOutputFormat)
Thread T80 created by T70 here:
    #0 0x56384a6ce1b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x56384b00fb32 in OTTripleGenerator<Spdz2kShare<64, 64>>::OTTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, Z2<64>, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:135:9
    #2 0x56384afea6d5 in NPartyTripleGenerator<Spdz2kShare<64, 64>>::NPartyTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, Z2<64>, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:40:9
    #3 0x56384afea6d5 in Spdz2kTripleGenerator<Spdz2kShare<64, 64>>::Spdz2kTripleGenerator(OTTripleSetup const&, Names const&, int, int, int, MascotParams&, Z2<64>, Player*) /home/lgp/MP-SPDZ/./OT/NPartyTripleGenerator.hpp:67:9
    #4 0x56384afea6d5 in OTPrep<Spdz2kShare<64, 64>>::set_protocol(SPDZ2k<Spdz2kShare<64, 64>>&) /home/lgp/MP-SPDZ/./Protocols/MascotPrep.hpp:44:28
    #5 0x56384afedc21 in Spdz2kPrep<Spdz2kShare<64, 64>>::set_protocol(SPDZ2k<Spdz2kShare<64, 64>>&) /home/lgp/MP-SPDZ/./Protocols/Spdz2kPrep.hpp:44:16
    #6 0x56384af7ce9c in SubProcessor<Spdz2kShare<64, 64>>::SubProcessor(MAC_Check_Z2k<Z2<128>, Z2<64>, Z2<128>, Spdz2kShare<64, 64>>&, Preprocessing<Spdz2kShare<64, 64>>&, Player&, ArithmeticProcessor*) /home/lgp/MP-SPDZ/./Processor/Processor.hpp:34:9

Thread T70 created by T0 here:
    #0 0x56384a6ce1b1 in pthread_create (/home/lgp/MP-SPDZ/spdz2k-party.x+0x32f1b1) (BuildId: 696978139b793a6e175ed0121529d7fe607437a3)
    #1 0x56384af3d4cd in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::prepare(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:203:7
    #2 0x56384af459b6 in Machine<Spdz2kShare<64, 64>, Share<gf2n_long>>::run(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>> const&) /home/lgp/MP-SPDZ/./Processor/Machine.hpp:452:3
    #3 0x56384a733c4c in int OnlineMachine::run<Spdz2kShare<64, 64>, Share<gf2n_long>>() /home/lgp/MP-SPDZ/./Processor/OnlineMachine.hpp:181:70
    #4 0x56384a72af4f in main /home/lgp/MP-SPDZ/Machines/spdz2k-party.cpp:45:5
    #5 0x7faf9681c082 in __libc_start_main /build/glibc-e2p3jK/glibc-2.31/csu/../csu/libc-start.c:308:16

==2455232==ABORTING
```