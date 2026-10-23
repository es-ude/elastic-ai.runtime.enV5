# Gyroscope and Acceleration Sensor

- **Type:** Bosch Sensortec BMI323
- **Usage:**
  - Measure the orientation based on a gyroscope
    (±125°/s, ±250°/s, ±500°/s, ±1000°/s, ±2000°/s)
  - Measure the acceleration (±2, ±4, ±8, ±16g)
  - Measure the temperature (-40°C to 85°C)
- **Data sheet:** [bst-bmi323-ds000.pdf](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf)
- **Header:** [Bmi323.h](./include/eai/sensor/Bmi323.h)

> [!NOTE]
> **Disregarded Functionality**
>
> Some additional functionality from the original BMI323 library,
> included from Bosch Sensortec, is not included in this facade.
>
> Additional functions provided by BMI323:
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
#include "CException.h"

#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/Spi.h"
#include "eai/hal/SpiTypedefs.h"
#include "eai/sensor/Bmi323.h"

// SPI bus configuration
spiConfiguration_t spi = {
  .spiInstance = BMI323_SPI_MODULE,
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
  Try {
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

```CMake title=CMakeLists.txt
add_executable(my-bmi323-app main.c)
target_link_libraries(my-bmi323-app PRIVATE
      Bmi323
      CException
      EnV5HwConfiguration
      Spi)
create_enV5_executable(my-bmi323-app)
```

## Further examples

[HardwareTestBmi323.c](../../../test/hardware/Sensors/HardwareTestBmi323.c)
provides a more sophisticated example of how to use this sensor.
