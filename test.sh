#! /bin/bash

if ! [[ -d "build_test" ]]; then
  mkdir "build_test"
fi

(
  cd build_test || exit

  cmake -DUNIT_TEST:BOOL=ON -DCMAKE_BUILD_TYPE=DEBUG -GNinja ..

  ninja -j 4

  fail_count=0

  for file in ./test/unit/unit-*; do
    printf "\n##################################################\n%s\n\n" "${file}"
    OUTPUT=$("./${file}")
    printf "%s\n" "${OUTPUT}"
    if [ "${OUTPUT: -2}" != OK ]; then
      ((fail_count = fail_count + 1))
    fi
  done

  if [ "$fail_count" == 0 ]; then
    printf "\n\033[0;32mAll tests succeeded\033[0;0m\n"
  else
    printf "\n\033[0;31m%s file(s) contain failing tests!!!\033[0;0m\n" "$fail_count"
  fi
)
