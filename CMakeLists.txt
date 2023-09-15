cmake_minimum_required(VERSION 3.23)
project(compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(compiler main.cpp Lexical.h Dependencies.h Syntactic.h debug.h ErrorHanding.h Quaternion.h Mips.h)
