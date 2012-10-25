cmake_minimum_required(VERSION 2.8)

project(opencl_info)

add_executable(opencl_info main.cpp)

TARGET_LINK_LIBRARIES(opencl_info OpenCL)