# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

#项目名称, 参数值是 Demo1, 该命令表示项目的名称是 Demo1
project(wine)

# 显示指定使用的C++编译器 
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS  "-g -finput-charset=GBK")

# 添加include路径
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(./src SrcFiles)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 指定生成目标
add_executable(wine ${SrcFiles})