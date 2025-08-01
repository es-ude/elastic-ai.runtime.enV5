# Temperature and humidity sensor

- Type: SHT3X
- [data sheet](href="https://www.mouser.de/pdfdocs/SHT3x-DIS.pdf)
- Usage:
  - Measure the current temperature
  - Measure the current humidity
- You can find the provided functionality in [Sht3x.h](include/eai/sensor/Sht3x.h).

## Basic usage example

```C title=main.c
#include "CException.h"
#include "hardware/i2c.h"

#include "Common.h"
#include "I2c.h"
#include "Sht3x.h"
#include "EnV5HwConfiguration.h"

// I2C bus configuration
i2cConfiguration_t i2cConfig = {
  .i2cInstance = I2C_INSTANCE,
  .frequency = I2C_FREQUENCY_IN_HZ,
  .sdaPin = I2C_SDA_PIN,
  .sclPin = I2C_SCL_PIN,
};

// sensor configuration
sht3xSensorConfiguration_t sensor = {
  .i2c_host = SHT_HOST,
  .i2c_slave_address = SHT_SLAVE,
};

int main(void) {
  i2cErrorCode = i2cInit(&i2cConfig); // initialize I2C bus
  if (i2cErrorCode != I2C_NO_ERROR) {
      PRINT("Initialise I2C failed!");
      return i2cErrorCode;
  }

  CEXCEPTION_T exception;
  Try {
      sht3xInit(sensor); // initialize sensor
      float temperature, humidity;
      sht3xGetTemperatureAndHumidity(&temperature, &humidity); // read values from sensor
  }
  Catch (exception) {
      return exception;
  }

  return 0;
}
```

```CMake title=CMakeLists.txt
add_executable(my-sht3x-app main.c)
target_link_libraries(my-sht3x-app
      pico_stdlib
      pico_stdio_usb
      hardware_i2c

      common_lib
      enV5_hw_configuration
      i2c_interface
      sensor_lib_sht3x
)
create_enV5_executable(my-sht3x-app)
```

[HardwaretestSht3x.c](../../../test/hardware/Sensors/HardwaretestSht3x.c)
provides a more sophisticated example of how to use this sensor.
