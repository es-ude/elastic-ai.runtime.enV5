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
    CEXCEPTION_t exception;

    // Initialize Sensor (ALWAYS REQUIRED)
    Try {
      pac193xInit(sensor);
    }
    Catch(exception){
        if (exception != PAC193X_NO_ERROR) {
          Throw(exception);
        }
    }

    // DO STUFF

    // Example: Read Values from Channel
    pac193xMeasurements_t measurements;

    Try {
      errorCode = pac193xGetMeasurementsForChannel(sensor, PAC193X_CHANNEL01, &measurements);
    }
    Catch(exception) {
      if (exception != PAC193X_NO_ERROR) {
        Throw(exception);
    }
    // ...

    return 0;
}
```

More detailed examples, on how to use this sensor, can be found
in [HardwaretestPac193x.c](../../../test/hardware/Sensors/HardwaretestPac193x.c).

