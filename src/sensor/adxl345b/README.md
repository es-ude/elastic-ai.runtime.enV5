# Acceleration Sensor

- Type: **ADXL345B**
- [Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf)
- Usage:
    - Measure the acceleration in x,y,z direction
- Provided functionality can be found in [Adxl345b.h](./include/Adxl345b.h)

## Basic Usage Example

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
in [HardwaretestADXL345b.c](../../../test/hardware/Sensors/HardwaretestAdxl345b.c).

