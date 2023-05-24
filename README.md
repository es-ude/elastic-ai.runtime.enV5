# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Setup

For compiling the project the tools are:

* required:
    * gcc (local C compiler)
    * arm-non-eabi-gcc (C compiler for Target device)
    * CMake (Build Script Generator)
    * Ninja (Build Tool)
* recommended:
    * pre-commit

First you need to load CMake once, if you use the CLion IDE as recommended, the IDE does that for you.
If you want to do this by yourself please refer to [CMake Profiles](##cmake_profiles).

To run the FPGA related flash scripts it is recommended to create a local virtual Python environment and install the
tool from the [requirements.txt](bitfile_scripts/requirements.txt).

### CMake Profiles

There are three CMake Profiles provided with the CLion settings: Debug, Release, and UnitTests.
The UnitTests profile only needs to be selected to run the unit tests.

The Debug and Release targets differ only in the amount of debug print information when the targets are run on a device.

Profiles:

```bash
# Unit-test profile
cmake -B cmake-build-debug -G Ninja -D CMAKE_BUILD_TYPE=DEBUG -D UNIT_TEST:BOOL=ON -D DEBUG_MODE:BOOL=ON

# Debug profile
cmake -B cmake-build-debug -G Ninja -D CMAKE_BUILD_TYPE=DEBUG -D UNIT_TEST:BOOL=OFF -D DEBUG_MODE:BOOL=ON

# Release profile
cmake -B cmake-build-release -G Ninja -D CMAKE_BUILD_TYPE=RELEASE -D UNIT_TEST:BOOL=OFF -D DEBUG_MODE:BOOL=OFF
```

The flag `-G Ninja` tells CMake to use Ninja as the build tool.
If this flag is not passed CMake will use the default build tool on your machine (mostly Makefiles).

### Configuration

In the [NetworkConfiguration.h](src/NetworkConfiguration.h) file the Network and MQTT connection settings can be
changed. To stop changes in these to be staged you should exclude them:

```bash
git update-index --assume-unchanged src/NetworkConfiguration.h
```

The same should be done for the CLion Settings stored in [.idea](.idea):

```bash
cd .idea
git ls-files -z | xargs -0 git update-index --assume-unchanged
```

## Target local machine

### Unit Tests

The unit-tests can be build and executed by running:

```bash
# build the unit-tests
cmake --build cmake-build-test -j 4 --clean-first

# execute the unit-tests
ctest --test-dir cmake-build-test/test/unit --output-on-failure
```

The build unit-tests can then be found under [cmake-build-test/test/unit](./cmake-build-test/test/unit) as executables.

## Target Pico

### Build all Targets

The debug targets can be built by executing:

```bash
cmake --build cmake-build-debug -j 4
```

The release targets can be built by executing:

```bash
cmake --build cmake-build-release -j 4
```

The `*.uf2` files to flash the pico can than be found in the [out](./out) folder.

### Build Demo

The main executable ([demo.c](src/Demo.c)) can be built with:

```bash
cmake --build cmake-build-release -j 4 --target main
```

The resulting `demo.uf2` file to flash the pico can be found in the [out](./out) folder.

### Hardware Tests

The hardware tests can be build using

```bash
cmake --build cmake-build-debug -j 4 --target <test_name>
```

replacing `<test_name>` with the name of the test.

The resulting `<test_name>.u2f` files to flash the pico can be found in the [out](./out) folder.

### CMD line output

If the pico is connected to the local machine the `print()` statements inside the code will be redirected to the USB and
is available as serial port output.
This output can be read via a serial port reader like screen, minicom
or [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

The following example shows how to use minicom on a Unix based system:

```bash
minicom -b 115200 -o -D /dev/ttyACM0
```

* `-b 115200` -> baud rate for connection
* `-D /dev/ttyACM0` -> serial port
    * differs depending on the host machine
    * can be found via `ls /dev/tty*` or `ls /dev/tty.*` (Linux / macOS)
* `-o` -> disable modem initialisation

### Debug Output

To enable enhanced Debug output add the flag `-D DEBUG_OUTPUT:BOOL=ON` to the cmake [setup](README.md#Setup) call (or
add it in the CLion CMake options).
This enables the `PRINT_DEBUG(...)` from common.h in all targets.

## MQTT Stress behavior

When MQTT messages are sent to fast to the device, some message will be dropped.

## FPGA Configuration

The FPGA on the ENv5 can be configured by writing a Bit- or Binfile to the flash.
To write a file to the flash, the ENv5 needs to be flashed with the hardware test `hardware-test_fpga_config` in
[test/hardware/TestEnv5Config](test/hardware/TestENv5Config/).
The Test only works on the ENv5, not the SensorBoard.
Put the Bit- or Binfile you want to send to the Device in the
directory [bitfile_scripts/bitfiles](bitfile_scripts/bitfiles) and execute the python
script [BitfileFlasher.py](bitfile_scripts/BitfileFlasher.py) with a number of arguments

* required: serial port of device `-p` or `--port`
* optionally: baudrate of serial connection `-b` or `--baudrate`
* 1st positional argument path to bitfile
* 2nd positional argument start flash address bitfile should be written at

The python script will send the Bitfile via serial to the ENv5 and afterwards verify that it's been written correctly.
To configure the FPGA with the new Bitfile, the FPGA has to be resetted (currently the FPGA will always reconfigure with
the config that starts at address 0x0).
The hardware test `hardware-test_fpga_config` can be used for this by sending the character 'r' via serial to the
device.

If your board does not contain a FPGA, use the hardware test `hardware-test_config`
in [test/hardware/TestConfiguration](test/hardware/TestConfiguration/) instead to test if the Bitfile Flashing worked.

### Known Problems

If the script fails repeatedly it's possible that the Bitfile currently in flash memory is wrong and the FPGA repeatedly
tries to reconfigure without success. It then blocks the flash until it is put into JTAG mode by shorting the 1x2
pinheader on the board, seen in the picture below.

![](/pics/jtag_header.jpg)

## Provided Sensor Libraries

### Power Sensor

- Type: **PAC193X**
- [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf)
- Usage:
    - Measure Power consumption of FPGA/Flash
    - Measure Power consumption of Wi-Fi module
- Provided Functionality can be found in [Pac193x.h](src/pac193x/Pac193x.h)

#### Basic Usage Example

```C
#include "pac193x/Pac193x.h"
#include "hardware/i2c.h"

pac193xSensorConfiguration_t sensor = {
    .i2c_host = i2c1,
    .i2c_slave_address = PAC193X_I2C_ADDRESS_499R,
    .powerPin = -1,
    .usedChannels = {.uint_channelsInUse = 0b00000011},
    .rSense = {0.82f, 0.82f, 0, 0},
};


int main(void) {
    // Initialize Sensor (ALWAYS REQUIRED)
    pac193xErrorCode_t errorCode = pac193xInit(sensor);
    if (errordCode != PAC193X_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read Values from Channel
    pac193xMeasurements_t measurements;
    errorCode = pac193xGetAllMeasurementsForChannel(sensor, PAC193X_CHANNEL01, &measurements);
    if (errordCode != PAC193X_NO_ERROR) {
        return errorCode;
    }
    // ...
    
    return 0;
}
```

More detailed examples, on how to use this sensor, can be found
in [HardwaretestPac193x.c](test/hardware/Sensors/HardwaretestPac193x.c).

### Temperature Sensor

- Type: **SHT3X**
- [Datasheet](https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf)
- Usage:
    - Measure the current temperature
    - Measure the current humidity
- Provided functionality can be found in [Sht3x.h](src/sht3x/Sht3x.h)

#### Basic Usage Example

```C
#include "sht3x/Sht3x.h"
#include "hardware/i2c.h"

int main(void) {
    // Initialize Sensor (ALWAYS REQUIRED)
    sht3xErrorCode_t errorCode = sht3xInit(i2c0);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read Temperature and Humidity
    float temperature, humidity;
    errorCode = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }
    
    // ...
    
    return 0;
}

```

More detailed examples, on how to use this sensor, can be found
in [HardwaretestSht3x.c](test/hardware/Sensors/HardwaretestSht3x.c).

### Acceleration Sensor

- Type: **ADXL345B**
- [Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf)
- Usage:
    - Measure the acceleration in x,y,z direction
- Provided functionality can be found in [Adxl345b.h](src/adxl345b/Adxl345b.h)

#### Basic Usage Example

```C
#include "adxl345b/Adxl345b.h"
#include "hardware/i2c.h"

int main(void) {
    // Initialize Sensor (ALWAYS REQUIRED)
    adxl345bErrorCode_t errorCode = adxl345bInit(i2c0, ADXL345B_I2C_ALTERNATE_ADDRESS);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read G value in x, y and z direction
    float xAxis, yAxis, zAxis;
    errorCode = adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);;
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    
    // ...
    
    return 0;
}
```

More detailed examples, on how to use this sensor, can be found
in [HardwaretestADXL345b.c](test/hardware/Sensors/HardwaretestAdxl345b.c).

## Submodules

Following submodules are being used

[es-ude/elastic-ai.runtime.c](https://github.com/es-ude/elastic-ai.runtime.c)

[FreeTROS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel)

[raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)

[ThrowTheSwitch/Unity](https://github.com/ThrowTheSwitch/Unity)

### Troubleshooting

* In case the command which fetches the submodules fails, try
  executing `git submodule update --init --recursive --force`. This will most likely fix the problem.
* To generate a clean CMake Build without deleting and reinitializing the build directory
  run `cmake --build <build_dir> --target clean`.
