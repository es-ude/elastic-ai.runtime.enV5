# Temperature and Humidity Sensor

- **Type:** SHT3X
- **Usage:**
  - Measure the current temperature
  - Measure the current humidity
- **Data sheet:** [SHT3x-DIS.pdf](https://www.mouser.de/pdfdocs/SHT3x-DIS.pdf)
- **Header:** [Sht3x.h](./include/eai/sensor/Sht3x.h)

## Basic usage example

```C title=main.c
#include "CException.h"

#include "eai/Common.h"
#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/I2c.h"
#include "eai/sensor/Sht3x.h"

// I2C bus configuration
i2cConfiguration_t i2cConfig = {
  .i2cInstance = I2C_MODULE,
  .frequency = I2C_FREQUENCY_IN_HZ,
  .sdaPin = I2C_SDA_PIN,
  .sclPin = I2C_SCL_PIN,
};

// sensor configuration
sht3xSensorConfiguration_t sensor = {
  .i2c_host = SHT_I2C_MODULE,
  .i2c_slave_address = SHT_SLAVE,
};

int main(void) {
  // initialize I2C bus
  i2cErrorCode_t i2cErrorCode = i2cInit(&i2cConfig);
  if (i2cErrorCode != I2C_NO_ERROR) {
    PRINT("Initialise I2C failed!");
    return i2cErrorCode;
  }

  CEXCEPTION_T exception;
  Try {
    sht3xInit(sensor); // initialize sensor
    float temperature, humidity;
    sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity); // read values from sensor
  }
  Catch (exception) {
    return exception;
  }

  return 0;
}
```

```CMake title=CMakeLists.txt
add_executable(my-sht3x-app main.c)
target_link_libraries(my-sht3x-app PRIVATE
      Sht3x
      Common
      CException
      EnV5HwConfiguration
      I2c)
create_enV5_executable(my-sht3x-app)
```

## Further examples

[HardwareTestSht3x.c](../../../test/hardware/Sensors/HardwareTestSht3x.c)
provides a more sophisticated example of how to use this sensor.
