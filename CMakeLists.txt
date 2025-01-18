# 
cmake_minimum_required(VERSION 3.0)
project(cmrpc)
set(CMAKE_CXX_STANDARD 11)  

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

link_directories(${PROJECT_SOURCE_DIR}/lib)

#  src包含所有 mprcp 框架所有的相关代码
add_subdirectory(src)
#  example 包含了mprac框架使用的 示例代码
add_subdirectory(example)