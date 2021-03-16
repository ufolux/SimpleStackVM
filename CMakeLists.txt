cmake_minimum_required(VERSION 3.16)
project(asmer C)

set(CMAKE_C_STANDARD 11)

add_executable(asmer assembler.c strmap.c ExecStack.c simplevm.cpp)