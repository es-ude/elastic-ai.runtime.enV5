# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Setup

For compiling the project the required tools are:

* gcc
* arm-non-eabi-gcc
* CMake
* Ninja

First you need to load CMake once (CLion does that for you):

```bash
cmake -B cmake-build-debug -G Ninja .
```

The flag `-G Ninja` tells CMake to use Ninja as the build tool.
If this flag is not passed CMake will use the default build tool on your machine (mostly Makefiles).

## Target local machine

### Unit Tests

To build and run the unit tests the [test.sh](test.sh) script can be used.

The tests can then be found under `cmake-build-debug/unit-test/test/unit` as executables.

## Target Pico

### Build Main

The main executable ([main.c](src/main.c)) can be build with:

```bash
cmake --build cmake-build-debug --target main
```

The resulting uf2 file to flash the pico can be found in the `out` folder.

### Hardware Tests

The hardware tests can be build using

```bash
cmake --build cmake-build-debug --target <test_name>
```

replacing `<test_name>` with the name of the test.

The resulting u2f files to flash the pico can be found in the `out` folder.

### CMD line output

If the pico is connected to the local machine the `print()` inside the code will be redirected to the USB and is
available as serial port output.
This output can be read via a serial port reader like screen,
minicom, [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

The following shows minicom as an example:

```bash
minicom -b 115200 -o -D /dev/ttyACM0
```

* `-b 115200` -> baud rate for connection
* `-D /dev/ttyACM0` -> serial port (maybe different on your machine)
* `-o` -> disable modem initialisation

## Debug Output

To enable enhanced Debug output add the flag `-D DEBUG_OUTPUT:BOOL=ON` to the cmake build call.
This enables the `PRINT_DEBUG(...)` from common.h in all targets.

## Submodules

Following submodules are being used

[es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)

[FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)

[raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

[ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)
