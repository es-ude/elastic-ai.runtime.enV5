# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Development

For development, for example in an IDE, the development flag can be set to include everything:

```
-DDEVELOP:BOOL=ON
```

## Build Main

The main executable ([main.c](src/main.c)) can be build with the [build.sh](build.sh) script.

The resulting uf2 file can be found in the build_pico/src directory.

## Unit Tests

To build and run the unit tests the [test.sh](test.sh) script can be used.

The test can then be found under build_test/test/unit as executables.

## Hardware Tests

The hardware tests can be build using the [hardware_test.sh](hardware_test.sh) script.

The resulting u2f files can be found under build_pico/test/hardware.

## Submodules

Following submodules are being used

[es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)

[FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)

[raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

[ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)
