# Power Sensor

- Type: **PAC193X**
- [Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf)
- Usage:
    - Measure Power consumption of the FPGA/Flash
    - Measure Power consumption of the Wi-Fi module
- Provided Functionality can be found in [Pac193x.h](./include/Pac193x.h)

## Basic Usage Example

```C
#include "pac193x/Pac193x.h"
#include "hardware/i2cConfig.h"

pac193xSensorConfiguration_t sensor = {
    .i2cConfig = i2c1,
    .i2cAddress = PAC193X_I2C_ADDRESS_499R,
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
    errorCode = pac193xGetMeasurementsForChannel(sensor, PAC193X_CHANNEL01, &measurements);
    if (errordCode != PAC193X_NO_ERROR) {
        return errorCode;
    }
    // ...

    return 0;
}
```

More detailed examples, on how to use this sensor, can be found
in [HardwaretestPac193x.c](../../../test/hardware/Sensors/HardwaretestPac193x.c).

