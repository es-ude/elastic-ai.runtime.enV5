#! /bin/bash

cmake --build cmake-build-test -j 4 --target clean

cmake --build cmake-build-release -j 4 --target clean
cmake --build cmake-build-debug -j 4 --target clean

rm -rf out/