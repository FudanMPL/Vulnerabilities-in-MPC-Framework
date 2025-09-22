# Vulnerabilities-in-MPC-Framework

In this project, we report our found vulnerabilities in MPC Frameworks, and provide instructions to reproduce the vulnerabilities. Most of these vulnerabilities are triggered by simulating an active adversary.

## Environment Requirement

OS: Ubuntu 

## Initial Step

Compile the library_recv.cpp using

```
cmake .
make
```

It will produce a librecv_hook.so, which will be used to simulate socket recv by file io.

If you have any questions, please email 1015277323@qq.com.
