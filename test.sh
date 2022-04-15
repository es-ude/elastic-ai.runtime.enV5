#! /bin/bash

if ! [[ -d "build_test" ]]; then
    mkdir "build_test"
fi

(
  cd build_test || exit

  cmake -DUNIT_TEST:BOOL=ON -DCMAKE_BUILD_TYPE=DEBUG -GNinja ..

  ninja -j4

  for file in /test/unit/unit-*
  do
    ./${file}
  done

  ninja clean
)
