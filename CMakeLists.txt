cmake_minimum_required(VERSION 3.12)

project(sand_pile)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RELEASE)

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(argument_parser/lib)
add_subdirectory(bmp_saver/lib)
