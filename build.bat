
cmake cobble_src -B build
cmake --build build
copy SDL2\lib\x64\*.dll build\Debug\
