# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Setup

The following commands in this section can be run using the shell script [setup.sh](setup.sh).

For the output of the builds, a directory needs to be created:

```bash
mkdir build_pico
```

The same holds true for the tests.

```bash
mkdir build_test
```

The project uses multiple submodules, which need to be initialized.

```bash
git submodule update --init --recursive
```

## Build

The following commands in this section can be run using the shell script [build.sh](build.sh).

Change into the build directory.

```bash
cd build_pico
```

Use cmake to configure the build process. (It will take some time on the first execution to download all necessary
files.)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS:bool=false ..
```

Then use make to build.

```bash
make -j4
```

The resulting uf2 file can then be found in the build directory.

Optionally you can clean up afterwards.

```bash
make clean
```

## Unit Tests

The following commands in this section can be run using the shell script [test.sh](test.sh).

First change into the test folder.

```bash
cd build_test
```

Use cmake to configure the tests.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS:bool=true ..
```

Build the tests using make.

```bash
make -j4
```

The test can now be found in the test folder under test/unit as executables.

Optionally you can clean up afterwards.

```bash
make clean
```

## Hardware Tests

The hardware tests are currently build together with the main and the resulting uf2 files can be found in the build
directory under test/hardware.

## Submodules

[es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c/tree/main)

[FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/main)

[raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

[ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity/tree/master)
