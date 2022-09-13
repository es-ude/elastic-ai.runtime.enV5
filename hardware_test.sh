#! /bin/bash

if ! [[ -d "build_pico" ]]; then
    mkdir "build_pico"
fi

(
  cd build_pico || exit

  cmake -DHARDWARE_TEST:BOOL=ON -DCMAKE_BUILD_TYPE=DEBUG -GNinja ..

  ninja -j4 hardware-test_fpga_config hardware-test_flash

  ninja clean
)
