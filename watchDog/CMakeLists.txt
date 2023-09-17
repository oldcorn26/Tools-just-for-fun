# 设置CMake最小版本要求
cmake_minimum_required(VERSION 3.16)

# 设置项目名称和语言
project(testCPP)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)

# 查找必要的线程库
find_package(Threads REQUIRED)

# 查找Boost库
find_package(Boost REQUIRED COMPONENTS system thread coroutine context)

# 查找OpenSSL库
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/libs)

# 添加可执行文件，并设置其源代码
add_executable(testCPP main.cpp)

# 链接线程库、Boost库以及OpenSSL库
target_link_libraries(testCPP PRIVATE Threads::Threads Boost::system Boost::thread Boost::coroutine Boost::context OpenSSL::SSL OpenSSL::Crypto)
