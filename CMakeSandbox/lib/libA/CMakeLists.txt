cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(myLib VERSION 1.0)

add_library(myLib STATIC src/lib.cpp)

target_include_directories(myLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

