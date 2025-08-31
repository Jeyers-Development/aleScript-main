#!/bin/bash

[ -d "build" ] && rm -rf "build"
mkdir -p build
cd build
cp -r ../resources resources
cmake ..
cmake --build .

./alescript ../program.ale