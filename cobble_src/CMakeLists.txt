cmake_minimum_required(VERSION 3.19)
project(cobble_src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)

## add extra include directories
include_directories("../SDL2/include")
## add extra lib directories from the library folder
link_directories("../SDL2/lib/x64")
#
## link from framework


find_library(sdl2 SDL2 PATHS "../SDL2/lib/x64")
#find_library(sdl2main SDL2main PATHS "../SDL2/lib/x64")
find_library(sdl2_image SDL2_image PATHS "../SDL2/lib/x64")
find_library(sdl2_gfx SDL2_gfx PATHS "../SDL2/lib/x64")
find_library(sdl2_ttf SDL2_ttf PATHS "../SDL2/lib/x64")

add_executable(cobble_src main.cpp projective_plane.h projective_plane.cpp cobble.h image_loader.h image_loader.cpp cobble.cpp graphic_utils.h screen_components.h screen_components.cpp)

#target_link_libraries(cobble_src sdl2 sdl2_image sdl2_gfx sdl2_ttf ${sdl2main})
target_link_libraries(cobble_src ${sdl2} ${sdl2_image} ${sdl2_gfx} ${sdl2_ttf} sdl2main)

