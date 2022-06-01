# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Setup

First you need to load cMake once (CLion does that for you):

```
cmake -B cmake-build-debug -G Ninja .
```

## Build Main

The main executable ([main.c](src/main.c)) can be build with:

```
cmake --build cmake-build-debug [-j 4] --target main
```

The resulting uf2 file can be found in the `out` folder.

## Unit Tests

To build and run the unit tests the [test.sh](test.sh) script can be used.

The test can then be found under build_test/test/unit as executables.

## Hardware Tests

The hardware tests can be build using, replacing `test_name` with the name of the test:

```
cmake --build cmake-build-debug [-j 4] --target test_name
```

The resulting u2f files can be found in the `out` folder.

## Submodules

Following submodules are being used

[es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)

[FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)

[raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

[ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)
