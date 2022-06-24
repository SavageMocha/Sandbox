cmake_minimum_required(VERSION 3.21)

add_library(MyLib STATIC src/lib.cpp include/lib.h)
target_include_directories(MyLib PUBLIC include)
