#define SOURCE_FILE "BMI323-SPI-ADAPTER"

/*!
 * Datasheet
 *    https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf
 */

#include <math.h>

#include "Sleep.h"
#include "Spi.h"
#include "SpiTypedefs.h"

#include "bmi3.h"
#include "bmi323_interface.h"

static BMI3_INTF_RET_TYPE bmi3_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
                                        void *intf_ptr) {
    data_t command = {.data = &reg_addr, .length = 1};
    data_t result = {.data = reg_data, .length = len};

    int readBytes = spiWriteCommandAndReadBlocking(intf_ptr, &command, &result);

    return readBytes == len ? BMI3_OK : BMI3_E_COM_FAIL;
}

static BMI3_INTF_RET_TYPE bmi3_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
                                         void *intf_ptr) {
    data_t command = {.data = &reg_addr, .length = 1};
    data_t data = {.data = reg_data, .length = len};

    int readBytes = spiWriteCommandAndDataBlocking(intf_ptr, &command, &data);

    return readBytes == len ? BMI3_OK : BMI3_E_COM_FAIL;
}

void bmi3_delay_us(uint32_t period, void *intf_ptr) {
    sleep_for_us(period);
}

int8_t bmi323Init(struct bmi3_dev *dev, spiConfiguration_t *spi) {
    if (dev == NULL) {
        return BMI3_E_NULL_PTR;
    }

    dev->read = bmi3_spi_read;
    dev->write = bmi3_spi_write;
    dev->intf = BMI3_SPI_INTF;
    dev->delay_us = bmi3_delay_us;
    dev->intf_ptr = spi;
    dev->read_write_len = 8;

    return bmi3_init(dev);
}

static float getMaxDpsForRange(uint8_t range) {
    switch (range) {
    case BMI3_GYR_RANGE_125DPS:
        return 125.0f;
    case BMI3_GYR_RANGE_250DPS:
        return 250.0f;
    case BMI3_GYR_RANGE_500DPS:
        return 500.0f;
    case BMI3_GYR_RANGE_1000DPS:
        return 1000.0f;
    case BMI3_GYR_RANGE_2000DPS:
        return 2000.0f;
    default:
        return 0;
    }
}
float bmi323LsbToDps(int16_t rawValue, uint8_t range, uint8_t resolution) {
    float halfScale = powf(2.0f, resolution) / 2.0f;
    float dpsRange = getMaxDpsForRange(range);
    return (dpsRange / halfScale) * ((float)rawValue);
}
