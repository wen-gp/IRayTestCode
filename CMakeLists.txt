cmake_minimum_required(VERSION 3.5)
message(STATUS "主CMake构建中")

#项目名称
project(Tbdp)

#设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(PLATFORM_NAME "aarch64-linux-gnu")
message(STATUS "Platform name is: ${PLATFORM_NAME}")

message(STATUS "CMAKE_BINARY_DIR：${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)#输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)#动态库
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)#静态库

message(STATUS "开始添加子项目")
# add_subdirectory(Src/TbdpApplication)
add_subdirectory(SDK/IRay/libir_sample/simple_sample/cmd_camera/linux/uvc_usb)
message(STATUS "子项目添加完毕")
message(STATUS "主CMake构建完毕")
