#! /bin/bash

cmake -B cmake-build-debug -D UNIT_TEST:BOOL=OFF -D DEBUG_OUTPUT:BOOL=ON -D CMAKE_BUILD_TYPE=DEBUG -G Ninja

cmake --build cmake-build-debug -j 4