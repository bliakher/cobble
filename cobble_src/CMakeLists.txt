cmake_minimum_required(VERSION 3.20)
project(cobble_src)

set(CMAKE_CXX_STANDARD 17)

# add extra include directories
include_directories(/usr/local/include)
# add extra lib directories from the library folder
link_directories(/usr/local/lib)

#find_library(sdl2 SDL2)
# link from framework
find_library(sdl2_image SDL2_image)
find_library(sdl2_gfx SDL2_gfx)
find_library(sdl2_ttf SDL2_ttf)


# link from /usr/local/include
find_package(SDL2 REQUIRED)

add_executable(cobble_src main.cpp projective_plane.h projective_plane.cpp cobble.h image_loader.h image_loader.cpp cobble.cpp graphic_utils.h screen_components.h screen_components.cpp file_utils.h)

target_link_libraries(cobble_src SDL2)

target_link_libraries(cobble_src ${sdl2_image} ${sdl2_gfx} ${sdl2_ttf})

