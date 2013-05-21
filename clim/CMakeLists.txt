cmake_minimum_required(VERSION 2.8)

project(clim)

add_executable(clim main.cpp)

TARGET_LINK_LIBRARIES(clim OpenCL)
TARGET_LINK_LIBRARIES(clim Magick++)

install(TARGETS clim RUNTIME DESTINATION bin)