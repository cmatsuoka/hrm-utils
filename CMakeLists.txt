cmake_minimum_required(VERSION 2.8.10)

project(hrm_utils)

include_directories(include)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(hasm)
add_subdirectory(emu)
