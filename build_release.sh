#! /bin/bash

cmake -B cmake-build-release -D UNIT_TEST:BOOL=OFF -D DEBUG_OUTPUT:BOOL=OFF -D CMAKE_BUILD_TYPE=RELEASE -G Ninja

cmake --build cmake-build-release -j 4
