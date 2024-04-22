cmake_minimum_required(VERSION 3.23)
project(socket_hook)
set(CMAKE_CXX_COMPILER "clang++") # Set the compiler to clang++

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 17)


add_library(recv_hook SHARED library_recv.cpp)



