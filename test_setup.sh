#!/usr/bin/env bash

if [[ -z $(command -v gcc) ]]; then
  UNDEFINED_DEPENDENCY_FOUND=1
  echo "gcc not found! Please install gcc compiler."
fi

if [[ -z $(command -v arm-none-eabi-gcc) ]]; then
  UNDEFINED_DEPENDENCY_FOUND=1
  echo "arm-none-eabi-gcc not found! Please install arm-none-eabi-gcc compiler."
fi

if [[ -z $(command -v cmake) ]]; then
  UNDEFINED_DEPENDENCY_FOUND=1
  echo "CMake not found! Please install CMake."
elif [[ -z $(cmake --version | awk '/cmake version/ && ($3 >= 3.13) {print "match"}') ]]; then
  UNDEFINED_DEPENDENCY_FOUND=1
  echo "Wrong CMAKE version installed!, Please install CMake version >3.13."
fi

if [[ -z $(command -v ninja) ]]; then
  UNDEFINED_DEPENDENCY_FOUND=1
  echo "Ninja not found! Please install Ninja."
fi

if [[ $UNDEFINED_DEPENDENCY_FOUND -ne 1 ]]; then
  echo "All dependencies are satisfied!"
fi
