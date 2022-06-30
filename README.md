# elastic-ai.runtime.enV5

Elastic AI implementation for the Elastic Node version 5.

## Setup

For compiling the project the tools are:
* required:
    * gcc
    * arm-non-eabi-gcc
    * CMake
    * Ninja
* recommended:
  * pre-commit

First you need to load CMake once (CLion does that for you):

```bash
cmake -B cmake-build-debug -G Ninja .
```

The flag `-G Ninja` tells CMake to use Ninja as the build tool.
If this flag is not passed CMake will use the default build tool on your machine (mostly Makefiles).

## Configuration

In the [configuration.h](src/configuration.h) file the Network and MQTT connection settings can be change. To stop
changes in these to be committed you can do:

```bash
git update-index --assume-unchanged src/configuration.h
```

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

To enable enhanced Debug output add the flag `-D DEBUG_OUTPUT:BOOL=ON` to the cmake [setup](README.md#Setup) call (or
add it in the CLion CMake options).
This enables the `PRINT_DEBUG(...)` from common.h in all targets.

## MQTT Stress behavior

When MQTT messages are sent to fast to the device, some message will be dropped.

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
pac193x_usedChannels usedChannels = {.uint_channelsInUse = 0b00000011};

int main(void) {
    // Initialize Sensor (ALWAYS REQUIRED)
    pac193x_errorCode errorCode = pac193x_init(i2c1, resistanceValues, usedChannels);
    if (errordCode != PAC193X_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read Values from Channel
    pac193x_measurements measurements;
    errorCode = pac193x_getAllMeasurementsForChannel(PAC193X_CHANNEL_SENSORS, &measurements);
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
    sht3x_errorCode errorCode = sht3x_init(i2c0);
    if (errorCode != SHT3X_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read Temperature and Humidity
    float temperature, humidity;
    errorCode = sht3x_getTemperatureAndHumidity(&temperature, &humidity);
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
    adxl345b_errorCode errorCode = adxl345b_init(i2c0, ADXL345B_I2C_ALTERNATE_ADDRESS);
    if (errorCode != ADXL345B_NO_ERROR) {
        return errorCode;
    }
    
    // DO STUFF
    
    // Example: Read G value in x, y and z direction
    float xAxis, yAxis, zAxis;
    errorCode = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);;
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
