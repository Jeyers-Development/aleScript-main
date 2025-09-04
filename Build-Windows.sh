#!/bin/bash

[ -d "build" ] && rm -rf "build"
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake
cmake --build .

WINEDEBUG=-all wine ./alescript.exe ../program.ale