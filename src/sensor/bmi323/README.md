# Gyroscope and acceleration sensor

- Type: Bosch Sensortec BMI323
- Usage:
  - Measure the Orientation based on a Gyroscope
    ( ±125°/s, ±250°/s, ±500°/s, ±1000°/s, ±2000°/s)
  - Measure the Acceleration (±2, ±4, ±8, ±16g)
  - Measure the Temperature (-40°C to 85°C)
- [data sheet](href="https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf)
- You can find the provided functionality in [Bmi323.h](./include/Bmi323.h).

> [!NOTE]  
> **Disregarded Functionality**
>
> Some additional functionality from the original BMI323 library,
> included from Bosch Sensortec, is not included in this facade.
>
> Additional Functions provided by BMI323:
>
> - Context Switch
> - FIFO buffer watermark
> - Get Error Status
> - Enhanced Flexibility
> - I3C based communication
> - Alternate Register Access
> - Get digital gain Data
> - Get Fast Offset Compensation (FOC) Data

## Basic usage example

```C title=main.c
#include "Bmi323.h"
#include "Spi.h"
#include "SpiTypedefs.h"
#include "EnV5HwConfiguration.h"
#include "CException.h"

// SPI bus configuration
spiConfiguration_t spi = {
  .spiInstance = BMI323_SPI_INTERFACE,
  .misoPin = BMI323_SPI_MISO,
  .mosiPin = BMI323_SPI_MOSI,
  .sckPin = BMI323_SPI_CLOCK,
  .csPin = BMI323_SPI_CHIP_SELECT,
  .baudrate = BMI323_SPI_BAUDRATE,
};

// sensor configuration
bmi323SensorConfiguration_t bmi323 = {0};

int main(void) {
  CEXCEPTION_T exception;
  Try  {
      spiInit(&spi); // initialize SPI bus
      bmi323Init(&bmi323, &spi); // initialize sensor

      uint8_t chipId[2];
      data_t idData = {.data = chipId, .length = sizeof(chipId)};
      bmi323GetRegister(&bmi323, BMI3_REG_CHIP_ID, &idData); // read sensor ID
  }
  Catch (exception) {
      return exception;
  }

  return 0;
}
```

```CMake
add_executable(my-bmi323-app main.c)
target_link_libraries(my-bmi323-app
      hardware_spi
      CException
      enV5_hw_configuration
      spi_interface
      sensor_lib_bmi323)
create_enV5_executable(my-bmi323-app)
```

[HardwaretestBmi323.c](../../../test/hardware/Sensors/HardwaretestBmi323.c)
provides a more sophisticated example of how to use this sensor.
