# Temperature Sensor

- Type: **SHT3X**
- [Datasheet](https://www.mouser.de/pdfdocs/SHT3x-DIS.pdf)
- Usage:
    - Measure the current temperature
    - Measure the current humidity
- Provided functionality can be found in [Sht3x.h](include/Sht3x.h)

## Basic Usage Example

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
in [HardwaretestSht3x.c](../../../test/hardware/Sensors/HardwaretestSht3x.c).
