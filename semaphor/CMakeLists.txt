# 设置CMake最小版本要求
cmake_minimum_required(VERSION 3.16)

# 设置项目名称和语言
project(testCPP)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找必要的线程库
find_package(Threads REQUIRED)

# 添加可执行文件，并设置其源代码
add_executable(testCPP main.cpp)

# 链接线程库
target_link_libraries(testCPP PRIVATE Threads::Threads)
