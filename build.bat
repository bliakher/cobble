cmake cobble_src -B build
cmake --build build --config Release
copy SDL2\lib\x64\*.dll build\Release\
xcopy cobble_src\data build\Release\data\ /E
