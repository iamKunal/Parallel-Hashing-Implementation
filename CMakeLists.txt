cmake_minimum_required(VERSION 3.8)
project(Parallel-Hashing-Implementation)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")

set(SOURCE_FILES main.cpp main2.cpp)
add_executable(Parallel-Hashing-Implementation ${SOURCE_FILES})
