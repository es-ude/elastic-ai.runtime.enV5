#! /bin/bash

(
  cd build_pico || exit

  cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS:bool=false ..

  make -j4
)

