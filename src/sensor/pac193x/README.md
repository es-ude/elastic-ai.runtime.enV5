# Power Sensor

- **Type:** PAC193X
- **Usage:**
  - Measure the power consumption of the FPGA/Flash
  - Measure the power consumption of the Wi-Fi module
- **Data sheet:** [PAC1931-Family-Data-Sheet-DS20005850E.pdf](https://ww1.microchip.com/downloads/en/DeviceDoc/PAC1931-Family-Data-Sheet-DS20005850E.pdf)
- **Header:** [Pac193x.h](./include/eai/sensor/Pac193x.h)

## Basic usage example

```C title=main.c
#include "eai/sensor/Pac193x.h"

// sensor configuration
pac193xSensorConfiguration_t sensor = {
  .i2cConfig = i2c1,
  .i2cAddress = PAC193X_I2C_ADDRESS_499R,
  .powerPin = -1,
  .usedChannels = {.uint_channelsInUse = 0b00000011},
  .rSense = {0.82f, 0.82f, 0, 0},
};

int main(void) {
  // power up sensor (ALWAYS REQUIRED)
  pac193xErrorCode_t errorCode =  pac193xPowerUpSensor(sensor);
  if (errorCode != PAC193X_NO_ERROR) {
    return errorCode;
  }

  // initialize sensor (ALWAYS REQUIRED)
  pac193xErrorCode_t errorCode = pac193xInit(sensor);
  if (errorCode != PAC193X_NO_ERROR) {
    return errorCode;
  }

  // read values from channel
  pac193xMeasurements_t measurements;
  errorCode = pac193xGetMeasurementsForChannel(sensor, PAC193X_CHANNEL01, &measurements);
  if (errorCode != PAC193X_NO_ERROR) {
    return errorCode;
  }

  return 0;
}
```

```CMake title=CMakeLists.txt
add_executable(my-pac193x-app main.c)
target_link_libraries(my-pac193x-app PRIVATE
      Pac193x
      I2c)
create_enV5_executable(my-pac193x-app)
```

## Further examples

[HardwareTestPac193x.c](../../../test/hardware/Sensors/HardwareTestPac193x.c)
provides a more sophisticated example of how to use this sensor.
