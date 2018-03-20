cmake_minimum_required(VERSION 2.8)

PROJECT(zero_copy)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(zero_copy MACOSX_BUNDLE zero_copy.cpp)

target_link_libraries(zero_copy ${VTK_LIBRARIES})

