#!/bin/bash

# delete old build folder
rm -rf ./build
# configure cmake project to build web version
emcmake cmake -G "Unix Makefiles" -S . -B ./build -DCMAKE_EXE_LINKER_FLAGS="-s USE_GLFW=3 -sMAX_WEBGL_VERSION=2 --embed-file shaders/default.frag --embed-file shaders/default.vert -s EXPORTED_RUNTIME_METHODS=['lengthBytesUTF8','stringToUTF8']"
# compile source code to web version
cmake --build ./build/