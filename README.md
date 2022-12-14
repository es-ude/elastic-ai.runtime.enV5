# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Setup

For compiling the project the tools are:

* required:
    * gcc
    * arm-non-eabi-gcc
    * CMake
    * ninja (or ninja-build)
* recommended:
    * pre-commit

First you need to load CMake once (CLion does that for you):

```bash
cmake -B cmake-build-debug -G Ninja .
```

The flag `-G Ninja` tells CMake to use Ninja as the build tool.
If this flag is not passed CMake will use the default build tool on your machine (mostly Makefiles).

## Configuration

In the [network_configuration.h](src/NetworkConfiguration.h) file the Network and MQTT connection settings can be
change. To stop changes in these to be committed you can do:

```bash
git update-index --assume-unchanged src/network_configuration.h
```

## Target local machine

### Unit Tests

To build and run the unit tests the [run_unittest.sh](./run_unittest.sh) script can be used. The tests can then be found under [cmake-build-test/test/unit](./cmake-build-test/test/unit) as executables.

## Target Pico

### Build all Targets

To build all targets at once the [build_debug.sh](./build_debug.sh) script can be used to generate builds with debug output enabled. If you don't want the debug output enabled use the [build_release.sh](./build_release.sh) script.

The `*.uf2` files to flash the pico can than be found in the [out](./out) folder.

### Build Main

The main executable ([main.c](src/Main.c)) can be build with:

```bash
cmake --build cmake-build-debug --target main
```

The resulting `main.uf2` file to flash the pico can be found in the [out](./out) folder.

### Hardware Tests

The hardware tests can be build using

```bash
cmake --build cmake-build-debug --target <test_name>
```

replacing `<test_name>` with the name of the test.

The resulting `<test_name>.u2f` files to flash the pico can be found in the [out](./out) folder.

### CMD line output

If the pico is connected to the local machine the `print()` inside the code will be redirected to the USB and is
available as serial port output.
This output can be read via a serial port reader like screen, minicom or [putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

The following example shows how to use minicom on a unix based system:

```bash
minicom -b 115200 -o -D /dev/ttyACM0
```

* `-b 115200` -> baud rate for connection
* `-D /dev/ttyACM0` -> serial port
  * differs depending on the host machine
  * can be found via `ls /dev/tty*` or `ls /dev/tty.*` (Linux /  macOS)
* `-o` -> disable modem initialisation

### Debug Output

To enable enhanced debug output add the flag `-D DEBUG_OUTPUT:BOOL=ON` to the cmake [setup](README.md#Setup) call (or add it in the CLion CMake options). This enables the `PRINT_DEBUG(...)` from common.h in all targets.

## MQTT Stress behavior

When MQTT messages are sent to fast to the device, some message will be dropped.

## FPGA Configuration

The FPGA on the ENv5 can be configured by writing a Bit- or Binfile to the flash.
To write a file to the flash, the ENv5 needs to be flashed with the hardware test `hardware-test_fpga_config` in
[test/hardware/TestEnv5Config](test/hardware/TestENv5Config/).
The Test only works on the ENv5, not the SensorBoard.
Put the Bit- or Binfile you want to send to the Device in the directory [bitfile_scripts/bitfiles](bitfile_scripts/bitfiles) and execute the python script [BitfileFlasher.py](bitfile_scripts/BitfileFlasher.py) with a number of arguments

* required: serial port of device `-p` or `--port`
* optionally: baudrate of serial connection `-b` or `--baudrate` 
* 1st positional argument path to bitfile
* 2nd positional argument start flash address bitfile should be written at

The python script will send the Bitfile via serial to the ENv5 and afterwards verify that it's been written correctly. To configure the FPGA with the new Bitfile, the FPGA has to be resetted (currently the FPGA will always reconfigure with the config that starts at address 0x0).
The hardware test `hardware-test_fpga_config` can be used for this by sending the character 'r' via serial to the device. 

If your board does not contain a FPGA, use the hardware test `hardware-test_config` in [test/hardware/TestConfiguration](test/hardware/TestConfiguration/) instead to test if the Bitfile Flashing worked.

### Known Problems

If the script fails repeatedly it's possible that the Bitfile currently in flash memory is wrong and the FPGA repeatedly tries to reconfigure without success. It then blocks the flash until it is put into JTAG mode by shorting the 1x2 pinheader on the board, seen in the picture below.

![](/pics/jtag_header.jpg)

## Sensors

### Power Sensor

- Type: **PAC193X**
- [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf)
- Usage:
    - Measure Power consumption of sensor array
    - Measure Power consumption of WiFi module
- Provided Functionality can be found in `src/pac193x/pac193x_public.h`

#### Basic Usage Example

```C
#include "pac193x/pac193x_public.h"
#include "hardware/i2c.h"

float resistanceValues[4] = {0.82f, 0.82f, 0, 0};
pac193xUsedChannels_t usedChannels = {.uint_channelsInUse = 0b00000011};

int main(void) {
    // Initialize Sensor (ALWAYS REQUIRED)
    pac193xErrorCode_t errorCode = pac193xInit(i2c1, resistanceValues, usedChannels);
    if (errordCode != PAC193X_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read Values from Channel
    pac193xMeasurements_t measurements;
    errorCode = pac193xGetAllMeasurementsForChannel(PAC193X_CHANNEL_SENSORS, &measurements);
    if (errordCode != PAC193X_NO_ERROR) {
        return errorCode;
    }
    // ...
    
    return 0;
}
```

More detailed examples, on how to use this sensor, can be found in `test/hardware/Sensors/test_pac193x.c`.

### Temperature Sensor

- Type: **SHT3X**
- [Datasheet](https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT3x_Datasheet_digital.pdf)
- Usage:
    - Measure the current temperature
    - Measure the current humidity
- Provided functionality can be found in `src/sht3x/sht3x_public.h`

#### Basic Usage Example

```C
#include "sht3x/sht3x_public.h"
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

More detailed examples, on how to use this sensor, can be found in `test/hardware/Sensors/test_sht3x.c`.

### Acceleration Sensor

- Type: **ADXL345B**
- [Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf)
- Usage:
    - Measure the acceleration in x,y,z direction
- Provided functionality can be found in `src/adxl345b/adxl345b_public.h`

#### Basic Usage Example

```C
#include "adxl345b/adxl345b_public.h"
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

More detailed examples, on how to use this sensor, can be found in `test/hardware/Sensors/test_adxl345b.c`.

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
