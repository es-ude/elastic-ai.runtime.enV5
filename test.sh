#! /bin/bash

(
  cd build_test || exit

  cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS:bool=true ..

  make -j4

  for file in /test/unit/unit-*
  do
    ./${file}
  done

  make clean
)
