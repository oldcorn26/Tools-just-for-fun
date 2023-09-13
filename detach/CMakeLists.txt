cmake_minimum_required(VERSION 3.16)
project(testCPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(testCPP main.cpp)

# 链接pthread库
find_package(Threads REQUIRED)
target_link_libraries(testCPP PRIVATE Threads::Threads)
