# Vulnerabilities-in-MPC-Framework

In this project, we report our found vulnerabilities in MPC Frameworks by simulating an active adversary, and provide instructions to reproduce the vulnerabilities.

## Environment Requirement

OS: Ubuntu 

## Initial Step

Compile the library_recv.cpp using

```
cmake .
make
```

It will produce a librecv_hook.so, which will be used to simulate socket recv by file io.