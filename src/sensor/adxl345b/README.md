# Acceleration Sensor

- Type: ADXL345B
- Usage: Measure the acceleration in x,y,z direction
- [data sheet](href="https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf)
- You can find the provided functionality under [Adxl345b.h](./include/Adxl345b.h).

## Basic usage example

```C title=main.c
#include "adxl345b/Adxl345b.h"
#include "hardware/i2cConfig.h"

int main(void) {
  // initialize sensor
  adxl345bErrorCode_t errorCode = adxl345bInit(i2c0, ADXL345B_I2C_ALTERNATE_ADDRESS);
  if (errorCode != ADXL345B_NO_ERROR) {
      return errorCode;
  }

  // read sensor values
  float xAxis, yAxis, zAxis;
  errorCode = adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);
  if (errorCode != ADXL345B_NO_ERROR) {
      return errorCode;
  }

  return 0;
}
```

```CMake title=CMakeLists.txt
add_executable(my-adxl345b-app main.c)
target_link_libraries(my-adxl345b-app
      hardware_i2c
      sensor_lib_adxl345b)
create_enV5_executable(my-adxl345b-app)
```

[HardwaretestADXL345b.c](../../../test/hardware/Sensors/HardwaretestAdxl345b.c)
provides a more sophisticated example of how to use this sensor.
