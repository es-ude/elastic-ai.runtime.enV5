#! /bin/bash

cmake -B cmake-build-debug/unit_test -D UNIT_TEST:BOOL=ON -D CMAKE_BUILD_TYPE=DEBUG -G Ninja .

cmake --build cmake-build-debug/unit_test -j 4

ctest --test-dir cmake-build-debug/unit_test/test/unit --output-on-failure
