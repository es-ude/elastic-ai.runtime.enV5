#! /bin/bash

cmake -B cmake-build-test -D UNIT_TEST:BOOL=ON -D DEBUG_OUTPUT:BOOL=ON -D CMAKE_BUILD_TYPE=DEBUG -G Ninja .

cmake --build cmake-build-test -j 4 --clean-first

ctest --test-dir cmake-build-test/test/unit --output-on-failure
