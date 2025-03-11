[![PR-Tests](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/run_checks.yml/badge.svg?branch=main)](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/run_checks.yml)
[![Create Release](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/push_to_main.yml/badge.svg?branch=main)](https://github.com/es-ude/elastic-ai.runtime.enV5/actions/workflows/push_to_main.yml)

# elastic-ai.runtime.enV5

> [!IMPORTANT]
> If you want to create your own project with the enV5 platform please refer to
> the [base-project](https://github.com/es-ude/enV5-base-project) as a starting point.

This repository provides the Elastic-AI runtime implementation for the Elastic Node version 5 (enV5).
A modular hardware platform with FPGA integration.

> [!TIP]
> More information about this project can be found at the [project homepage](https://www.uni-due.de/es/elastic_ai.php).

## Set Up

For using this codebase, the minimal required tools are:

* gcc (local C compiler)
* arm-non-eabi-gcc (C compiler for RP2040)
* CMake (Build Script Generator)

A more detailed set up guide can be found at [documentation/setup_guide.md](documentation/SETUP_GUIDE.md).


> [!IMPORTANT]
> This Guide assumes you are using a UNIX-like System and have `git` already installed!

### Get the Source Code

At first, you have to download the source code from GitHub.
This can be archived by running

```bash
git clone https://github.com/es-ude/elastic-ai.runtime.enV5.git enV5  # Download the Repository
cd enV5                                                               # Move inside the repository
```

in your shell.

### System Requirements

To determine if all required tools are installed, it is recommended to run
the [test_setup script](./documentation/test_setup.sh) in
your shell.
If you are missing some tools, this script will tell you the missing ones.

#### Required Tools

* **[GCC](https://gcc.gnu.org/)**
  -> C Compiler
  Can be installed via your local package manager like dnf, apt or brew.
* **[ARM-None-EABI GCC](https://developer.arm.com/downloads/-/gnu-rm)**
  -> C Cross-Compiler for ARM
  Can be installed via your local package manager like dnf, apt or brew.
* **[CMake](https://cmake.org)**
  -> Build System
  Can be installed via your local package manager like dnf, apt or brew.
* **[Ninja](https://ninja-build.org/)**
  -> Build Tool
  Can be installed via your local package manager like dnf, apt or brew.
* **[Astral-UV](https://docs.astral.sh/uv/) [OPTIONAL]**
  -> Python Runtime/Package Manager
  Ca be installed via your local package manager like dnf, apt or brew.

#### Python Environment [OPTIONAL]

> [!NOTE]
> The Python environment is only required if you want to use the provided python utils to interact with the enV5 Node.

To minimize the potential corruption of your system we recommend installing the dependencies inside
a [virtual environment](https://python.land/virtual-environments/virtualenv#How_to_create_a_Python_venv).
The python environment can be created and prepared by running:

```bash
uv venv .venv              # create a virtual python environment under `.venv/`
source .venv/bin/activate  # load the virtual environment
uv sync                    # install all python dependencies
```

> [!TIP]
> To use pre-commit for [conventional commit checks](documentation/CONTRIBUTION.md#contribution-guidelines) run
> ```bash
> uv run pre-commit init
> ```
> after initializing the Python environment

> [!IMPORTANT]
> To be able to use pre-commit and the other python tools you have to load the virtual python environment or execute the
> script with uv
> > [!TIP]
> > Use a tool like [direnv](https://direnv.net/) to automatically set up your environment when entering the folder
> > inside your shell!

### Compile the Source Code

> [!IMPORTANT]
> You have to adjust the network/mqtt broker credentials as mentioned
> in [network README](src/network/README.adoc)!

> [!NOTE]
> This project is based on CMake.

We provide three predefined profiles with the [CMakePresets.json](CMakePresets.json):

* host
* env5_rev2
* env5_rev2_debug

These profiles provide the required settings to either build for your local system (_host_) to locally run test,
or to build for the enV5 Node (_env5_rev2_ and _env5_rev_2_debug_).
The profile with the suffix _debug_ also provides additional console output for debugging purposes.

To initialize the CMake run:

```bash
cmake --preset host
cmake --preset env5_rev2
cmake --preset env5_rev2_debug
```

> [!WARNING]
> This may take a while because all required external dependencies are loaded from the internet.
> See [external dependencies](#external-dependencies).

### Local Test Execution

The local execution of the unit tests is possible via the `ctest` function as follows:

```bash
ctest --preset unit_test
```

> [!NOTE]
> The unit-test executables can then be found under [build/host/test/unit](build/host/test/unit).

### enV5 Node Execution

The hardware test executables for the enV5 Node can be found under [build/env5_rev2](build/env5_rev2/test/hardware)
or [build/env5_rev2_debug](build/env5_rev2_debug/test/hardware).

The `*.uf2` files to flash the pico can then be found under their respective subfolders.

### Flash the enV5

1. Press and hold `MCU BOOT` on the enV5 Node
2. Press `MCU RST` on the enV5 Node
3. Release `MCU BOOT`
4. Copy the `*.uf2` File to the RPI-RP2 device
    * Via the USB device through your file manager
    * Via the command line by executing `sudo cp <file>.uf2 /dev/sdX`

### Get Output from the enV5 Node

To receive output from the enV5 you have to connect the enV5 to your local machine.
This can be accomplished by connecting the USB-C port of the enV5 with a USB port of your local machine.

Once the enV5 is connected to your computer, you can read the debug output from the device with a serial port reader
like screen, minicom or [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

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

in your shell.

> [!CAUTION]
> If you can’t receive any output from the enV5 the problem is possibly caused by a bug in the source code.
> If the serial output is not initialized properly (see integration tests), the enV5 is not able to send the output to
> your device!

## External Dependencies

> [!IMPORTANT]
> These dependencies will automatically be downloaded and initialized by CMake!

Following external dependencies are used by this project:

* [es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)
* [raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
* [FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)
* [ThrowTheSwitch/CException](https://github.com/ThrowTheSwitch/CException)
* [ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)

## Troubleshooting

* To generate a clean CMake Build without deleting and reinitializing the build directory run
  `cmake --build <build_dir> --target clean`.
* If the device doesn’t connect to the wifi or mqtt broker make sure that you set up the correct
  credentials! ([NetworkConfig.c](NetworkConfig.c))
* [Debugging the FPGA](src/fpga/Vivado_Debugger.adoc)

## Glossary

|                   Term |        Scope        | Description                                                                                          |
|-----------------------:|:-------------------:|:-----------------------------------------------------------------------------------------------------|
|                   enV5 |          –          | Hardware with the MCU, Flash, FPGA, …                                                                |
|                  Flash |          –          | Flash storage on the board                                                                           |
|                    HAL |          –          | Hardware Abstraction Layer   Libraries to bundle direct hardware dependencies   (uart, spi, gpio, …) |
| Config / Configuration |  FPGA, Middleware   | bin file used to configure the FPGA                                                                  |
|               FreeRTOS |       Network       | Open Source Real Time Operating System   Used to emulate Threads                                     |
|            ESP32 / ESP |       Network       | WiFi Module from espressif                                                                           |
|            AT Commands | Network, MQTT, HTTP | String based command set to control the ESP32 WiFi module                                            |
|                 Broker |    MQTT, Network    | MQTT User implementation   used to publish and subscribe to topics                                   |
