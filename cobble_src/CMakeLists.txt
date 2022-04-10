cmake_minimum_required(VERSION 3.20)
project(cobble_src)

set(CMAKE_CXX_STANDARD 17)

find_library(sdl2 SDL2)
find_library(sdl2_image SDL2_image)

add_executable(cobble_src main.cpp)

target_link_libraries(cobble_src ${sdl2})
target_link_libraries(cobble_src ${sdl2_image})
