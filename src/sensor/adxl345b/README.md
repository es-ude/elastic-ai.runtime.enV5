# Acceleration Sensor

- **Type:** ADXL345B
- **Usage:** Measure the acceleration in x, y and z direction
- **Data sheet:** [ADXL345.pdf](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf)
- **Header:** [Adxl345b.h](./include/eai/sensor/Adxl345b.h)

## Basic usage example

```C title=main.c
#include "eai/hal/I2c.h"
#include "eai/sensor/Adxl345b.h"

// I2C bus configuration
i2cConfiguration_t i2cConfig = {
  .i2cInstance = i2c1,
  .frequency = 400000,
  .sdaPin = 6,
  .sclPin = 7,
};

// sensor configuration
adxl345bSensorConfiguration_t sensor = {
  .i2c_slave_address = ADXL345B_I2C_ALTERNATE_ADDRESS,
  .i2c_host = i2c1,
};

int main(void) {
  // initialize I2C bus
  i2cErrorCode_t i2cErrorCode = i2cInit(&i2cConfig);
  if (i2cErrorCode != I2C_NO_ERROR) {
    return i2cErrorCode;
  }

  // initialize sensor
  adxl345bErrorCode_t errorCode = adxl345bInit(sensor);
  if (errorCode != ADXL345B_NO_ERROR) {
    return errorCode;
  }

  // read sensor values
  adxl345bRawData_t rawData;
  errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
  if (errorCode != ADXL345B_NO_ERROR) {
    return errorCode;
  }

  float xAxis, yAxis, zAxis;
  errorCode = adxl345bConvertDataXYZ(sensor, &xAxis, &yAxis, &zAxis, rawData);
  if (errorCode != ADXL345B_NO_ERROR) {
    return errorCode;
  }

  return 0;
}
```

```CMake title=CMakeLists.txt
add_executable(my-adxl345b-app main.c)
target_link_libraries(my-adxl345b-app PRIVATE
      Adxl345b
      I2c)
create_enV5_executable(my-adxl345b-app)
```

## Further examples

[HardwareTestAdxl345b.c](../../../test/hardware/Sensors/HardwareTestAdxl345b.c)
provides a more sophisticated example of how to use this sensor.
