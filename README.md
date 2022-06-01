# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Build Main

The main executable ([main.c](src/main.c)) can be build with:

```
cmake --build cmake-build-debug --target main [-j 4]
```

The resulting uf2 file can be found in the `out` folder.

## Unit Tests

To build and run the unit tests the [test.sh](test.sh) script can be used.

The test can then be found under build_test/test/unit as executables.

## Hardware Tests

The hardware tests can be build using, replacing `test_name` with the name of the test:

```
cmake --build cmake-build-debug --target test_name [-j 4]
```

The resulting u2f files can be found in the `out` folder.

## Submodules

Following submodules are being used

[es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)

[FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)

[raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

[ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)
