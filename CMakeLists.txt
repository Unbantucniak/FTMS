cmake_minimum_required(VERSION 3.21)

project(FTMS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置公共头文件路径
set(COMMON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common/include")
include_directories(${COMMON_INCLUDE_DIR})

# 添加子目录
add_subdirectory(backend)
add_subdirectory(frontend)
