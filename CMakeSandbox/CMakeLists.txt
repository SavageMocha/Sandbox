cmake_minimum_required(VERSION 3.21)

project(MyProject VERSION 1.0 DESCRIPTION "Very nice project" LANGUAGES CXX)

add_subdirectory(lib)

add_executable(MyExe main.cpp)
target_include_directories(MyExe PUBLIC include)

target_link_libraries(MyExe PUBLIC MyLib)
