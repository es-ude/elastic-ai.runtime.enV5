[![Tests](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/run_checks.yml/badge.svg)](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/run_checks.yml)
[![Create Release](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/push_to_main.yml/badge.svg)](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/push_to_main.yml)

# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5 (enV5).

## Setup

For compiling the project, the required tools are:

- gcc (local C compiler)
- arm-non-eabi-gcc (C compiler for RP2040) [macOS (brew): gcc-arm-embedded]
- CMake (Build Script Generator)
- Ninja (Build Tool)

We recommended pre-commit to check that the commit message align with the conventional commit mesage standard.

> You can check if your local machine satisfies the required dependencies by executing `test_setup.sh` script
> in the project [root directory](.).

After pulling the repository, you are required to download all submodules (see [Submodules](#submodules)).
This can be archived by executing 
```bash
git submodule update --init --recursive
```

First you need to load CMake once, if you use the CLion IDE as recommended, the IDE does that for you.
If you want to do this by yourself, please refer to [CMake Profiles](#cmake-profiles).

To run the FPGA related flash scripts, it is recommended to create a local virtual Python environment and install the
tools from the [requirements.txt](bitfile_scripts/requirements.txt) file.

### CMake Profiles

There are three CMake Profiles provided with the CLion settings: **Debug**, **Release**, and **UnitTests**.

The UnitTests profile only needs to be selected to run the unit tests.
The Debug and Release targets differ only in the amount of printed information when the targets are running on a device.

Profiles:

```bash
# Unit-test profile
cmake -B build/unit-tests -G Ninja -D CMAKE_BUILD_TYPE=DEBUG -D UNIT_TEST:BOOL=ON -D DEBUG_MODE:BOOL=ON

# Debug profile
cmake -B build/debug -G Ninja -D CMAKE_BUILD_TYPE=DEBUG -D UNIT_TEST:BOOL=OFF -D DEBUG_MODE:BOOL=ON

# Release profile
cmake -B build/release -G Ninja -D CMAKE_BUILD_TYPE=RELEASE -D UNIT_TEST:BOOL=OFF -D DEBUG_MODE:BOOL=OFF
```

The flag `-G Ninja` tells CMake to use Ninja as the build tool.
If this flag is not passed, CMake will use the default build tool on your machine (mostly Makefiles).

## Target local machine

### Unit Tests

The unit-tests can be build and executed by running:

```bash
# build the unit-tests
cmake --build build/unit-tests -j 4 --clean-first

# execute the unit-tests
ctest --test-dir build/unit-tests/test/unit --output-on-failure
```

The built unit-tests can then be found under [build/unit-tests/test/unit](./build/unit-tests/test/unit) as executables.

## Target Pico

### Build all Targets

The debug targets can be built by executing:

```bash
cmake --build build/debug -j 4
```

The release targets can be built by executing:

```bash
cmake --build build/release -j 4
```

The `*.uf2` files to flash the pico can than be found under the [out](./out) folder.

### Hardware Tests

The hardware tests can be build using

```bash
cmake --build build/debug -j 4 --target <test_name>
```

replacing `<test_name>` with the name of the test.

The resulting `<test_name>.u2f` files to flash the pico can be found under the [out](./out) folder.

### Flashing the Elastic Node version 5 (enV5)

1. Press and hold 'MCU BOOT' on the Elastic Node
2. Press 'MCU RST' on the Elastic Node
3. Release 'MCU BOOT'
4. Copy the `.uf2` File to the RPI-RP2 device

### CMD line output

If the pico is connected to the local machine the `printf()` statements inside the code will be redirected to the USB
and are available as serial port output.
This output can be read via a serial port reader like screen, minicom or [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

The following example shows how to use minicom on a Unix-based system:

```bash
minicom -b 115200 -o -D /dev/ttyACM0
```

- `-b 115200` -> baud rate for connection
- `-D /dev/ttyACM0` -> serial port
    - differs depending on the host machine
    - can be found via `ls /dev/tty*` or `ls /dev/tty.*` (Linux / macOS)
- `-o` -> disable modem initialisation

### Debug Output

To enable enhanced Debug output add the flag `-D DEBUG_OUTPUT:BOOL=ON` to the cmake [setup](README.md#Setup) call (or
add it in the CLion CMake options).
This enables the `PRINT_DEBUG(...)` from common.h in all targets.

## Submodules

Following submodules are being used

- [es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)
- [raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
- [FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)
- [ThrowTheSwitch/CExcpetion](https://github.com/ThrowTheSwitch/CException)
- [ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)

## Troubleshooting

- In case the command which fetches the submodules fails, try
  executing `git submodule update --init --recursive --force`. This will most likely fix the problem.
- To generate a clean CMake Build without deleting and reinitializing the build directory
  run `cmake --build <build_dir> --target clean`.
