# elastic-ai.runtime.enV5

<!--toc:start-->

- [elastic-ai.runtime.enV5](#elastic-airuntimeenv5)
  - [System Requirements](#system-requirements)
  - [Set Up](#set-up)
    - [Get the Source Code](#get-the-source-code)
    - [Setup devenv](#setup-devenv)
      - [Python Environment [OPTIONAL]](#python-environment-optional)
    - [Compile the Source Code](#compile-the-source-code)
    - [Local Test Execution](#local-test-execution)
    - [enV5 Node Execution](#env5-node-execution)
    - [Flash the enV5](#flash-the-env5)
    - [Get Output from the enV5 Node](#get-output-from-the-env5-node)
  - [External Dependencies](#external-dependencies)
  - [Troubleshooting](#troubleshooting)
  - [Glossary](#glossary)
  <!--toc:end-->

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/es-ude/elastic-ai.runtime.enV5/run_checks.yml)
[![GitHub Release](https://img.shields.io/github/v/release/es-ude/elastic-ai.runtime.enV5)](https://github.com/es-ude/elastic-ai.runtime.enV5/releases/latest)
[![View Python-Tutorial on GitHub](https://img.shields.io/github/stars/es-ude/elastic-ai.runtime.enV5?color=232323&label=elastic-ai.runtime.enV5&logo=github&labelColor=232323)](https://github.com/es-ude/elastic-ai.runtime-enV5)
[![Author es-ude](https://img.shields.io/badge/es-ude-b820f9?labelColor=b820f9&logo=githubsponsors&logoColor=fff)](https://github.com/es-ude)

> [!IMPORTANT]
> If you want to create your own project with the enV5 platform refer to
> the [base-project](https://github.com/es-ude/enV5-base-project) as a starting point.

This repository provides the Elastic-AI runtime implementation for the Elastic Node version 5 (enV5).
A modular hardware platform with FPGA integration.

> [!TIP]
> More information about this project can be found at the [project homepage](https://www.uni-due.de/es/elastic_ai.php).

## System Requirements

- **[devenv](https://devenv.sh/getting-started/)**
  -> Tool for reproducible development environments
- **[GCC](https://gcc.gnu.org/)** (through devenv)
  -> C Compiler
- **[ARM-None-EABI GCC](https://developer.arm.com/downloads/-/gnu-rm)** (through devenv)
  -> C Cross-Compiler for ARM
- **[CMake](https://cmake.org)** (through devenv)
  -> Build System
- **[Ninja](https://ninja-build.org/)** (through devenv)
  -> Build Tool
- **[Astral-UV](https://docs.astral.sh/uv/)** (through devenv)
  -> Python Runtime/Package Manager

## Set Up

> [!IMPORTANT]
> This Guide assumes you are using a devenv compatible OS.

### Get the Source Code

At first, you have to download the source code from GitHub.
This can be archived by running

```bash
git clone https://github.com/es-ude/elastic-ai.runtime.enV5.git enV5  # Download the Repository
cd enV5                                                               # Move inside the repository
```

in your shell.

### Setup devenv

1. At first you have to install devenv.
   This can be archived by following their instructions in their [Getting-Started-Guide](https://devenv.sh/getting-started/).
2. Install [direnv](https://direnv.net/), to automatically load the development environment provided by devenv.

#### Python Environment [OPTIONAL]

> [!NOTE]
> The Python environment is only required if you want to use the provided python utilities to interact with the enV5 Node.

To minimize the potential corruption of your system we recommend installing the dependencies inside
a [virtual environment](https://python.land/virtual-environments/virtualenv#How_to_create_a_Python_venv).
The python environment can be created and prepared by running:

```bash
uv venv .venv              # create a virtual python environment under `.venv/`
uv sync --all-groups       # install all python dependencies
```

### Compile the Source Code

> [!IMPORTANT]
> You have to adjust the network/mqtt broker credentials as mentioned
> in [network README](src/network/README.adoc)!

> [!NOTE]
> This project is based on CMake.

We provide three predefined profiles with the [CMakePresets.json](CMakePresets.json):

- unit_test
- env5_rev2_release
- env5_rev2_debug

These profiles offer the required settings to either build for your local system (_unit_test_) to locally run test,
or to build for the enV5 Node (_env5_rev2_release_ and _env5_rev_2_debug_).
The profile with the suffix _debug_ also provides additional console output for debugging purposes.

To initialize the CMake Tool run:

```bash
# directly
cmake --preset unit_test
cmake --preset env5_rev2_debug
cmake --preset env5_rev2_release

# custom function provided by devenv
setup_cmake
```

> [!WARNING]
> Loading all required external dependencies from the internet can take a while.
> See [external dependencies](#external-dependencies).

### Local Test Execution

The local execution of the unit tests is possible by using the `ctest` function as follows:

```bash
# directly
ctest --preset unit_test

# as a task provided by devenv
devenv tasks run "check:unit-test"
```

> [!NOTE]
> You can then find the unit-test executables under [build/unit-test/test/unit](build/unit-test/test/unit).

### enV5 Node Execution

You can find the hardware test executables for the enV5 Node under [build/env5_rev2_release](build/env5_rev2/test/hardware) or [build/env5_rev2_debug](build/env5_rev2_debug/test/hardware).
You can then find the `*.uf2` files to flash the pico inside their directories.

### Flash the enV5

Run

```bash
# script provided by devenv
flash_node <path_to_uf2>
```

**OR**

1. Press and hold `MCU BOOT` on the enV5 Node
2. Press `MCU RST` on the enV5 Node
3. Release `MCU BOOT`
4. Copy the `*.uf2` File to the RPI-RP2 device
   - Via the USB device through your file manager
   - Via the command line by executing `sudo cp <file>.uf2 /dev/sdX`

### Get Output from the enV5 Node

To receive output from the enV5 you have to connect the enV5 to your local machine.
Connect the USB-C port of the enV5 with a USB port of your local machine to do this.

After you connect the enV5 to your computer, read the debug output from the device with a serial port reader such as screen, minicom, or [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

We recommend using minicom as it is the most versatile of the aforementioned tools.
You can receive the output of the device by executing

> [!IMPORTANT]
> The serial port differs depending on the host machine!
> It can be found via `ls /dev/tty*` (Linux) or `ls /dev/tty.*` (macOS) from the terminal.

```bash
minicom
-o \             #  Disable modem initialisation
-w \             # Enable line wrap
-b 115200 \      # Set the baud rate to 115200
-D /dev/ttyACM0  # Define the serial port to read
```

in your shell prompt.

> [!CAUTION]
> If you cannot receive any output from the enV5 the problem is possibly caused by a bug in the source code.
> If the serial output is not initialized properly (see integration tests), the enV5 is not able to send the output to your device!

## External Dependencies

> [!IMPORTANT]
> CMake will automatically download and initialize these dependencies!

This project uses the following external dependencies:

- [es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)
- [raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
- [FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)
- [ThrowTheSwitch/CException](https://github.com/ThrowTheSwitch/CException)
- [ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)

## Troubleshooting

- To generate a clean CMake Build without deleting and initializing the build directory again run `cmake --build --target clean --preset <preset_to_clean>`.
- If the device does not connect to the Wi-Fi or MQTT broker make sure that you set up the correct credentials! ([NetworkConfig.c](NetworkConfig.c))
- [Debugging the FPGA](src/fpga/Vivado_Debugger.adoc)

## Glossary

|                   Term |        Scope        | Description                                                                                      |
| ---------------------: | :-----------------: | :----------------------------------------------------------------------------------------------- |
|                   enV5 |          –          | Hardware with the MCU, Flash, FPGA, …                                                            |
|                  Flash |          –          | Flash storage on the board                                                                       |
|                    HAL |          –          | Hardware Abstraction Layer Libraries to bundle direct hardware dependencies (UART, SPI, GPIO, …) |
| Config / Configuration |  FPGA, Middleware   | bin file used to configure the FPGA                                                              |
|               FreeRTOS |       Network       | Open Source Real Time operating system Used to emulate Threads                                   |
|            ESP32 / ESP |       Network       | Wi-Fi Module from espressif                                                                      |
|            AT Commands | Network, MQTT, HTTP | String based command set to control the ESP32 Wi-Fi module                                       |
|                 Broker |    MQTT, Network    | MQTT User implementation used to publish and subscribe to topics                                 |
