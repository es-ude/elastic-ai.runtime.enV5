#! /bin/bash

if ! [[ -d "build_pico" ]]; then
    mkdir "build_pico"
fi

(
  cd build_pico || exit

  cmake -DCMAKE_BUILD_TYPE=DEBUG ..

  make -j4

  make clean
)
