#include "I2c.h"
#define INCLUDE_ENV5_I2C_INTERNAL_HEADER
#include "I2cInternal.h"
#include "I2cTypedefs.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <stdint.h>

/* region FUNCTION IMPLEMENTATIONS FROM HEADER FILE*/

void i2cInit(i2c_inst_t *i2cHost, uint32_t baudRate, uint8_t sdaGPIO, uint8_t sclGPIO) {
    i2c_init(i2cHost, baudRate);
    i2cInternalSetupSda(sdaGPIO);
    i2cInternalSetupScl(sclGPIO);
}

i2cErrorCode_t i2cWriteCommand(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                               uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    int successfulTransmit =
        i2cInternalWriteBlocking(commandBuffer, sizeOfCommandBuffer, slaveAddress, i2cHost);

    /* sensor not available */
    if (successfulTransmit == PICO_ERROR_GENERIC) {
        return I2C_INIT_ERROR;
    }
    /* all bytes successful send to sensor */
    if (successfulTransmit == sizeOfCommandBuffer) {
        return I2C_NO_ERROR;
    }

    /* ACK not received */
    return I2C_ACK_ERROR;
}

i2cErrorCode_t i2cReadData(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                           i2c_inst_t *i2cHost) {
    int successfulTransmit =
        i2cInternalReadBlocking(readBuffer, sizeOfReadBuffer, slaveAddress, i2cHost);

    /* sensor not available */
    if (successfulTransmit == PICO_ERROR_GENERIC) {
        return I2C_INIT_ERROR;
    }

    /* all bytes successful received from sensor */
    if (successfulTransmit == sizeOfReadBuffer) {
        return I2C_NO_ERROR;
    }

    /* ACK not received */
    return I2C_ACK_ERROR;
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static void i2cInternalSetupSda(uint8_t sdaGPIO) {
    gpio_set_function(sdaGPIO, GPIO_FUNC_I2C);
    gpio_pull_up(sdaGPIO);
}

static void i2cInternalSetupScl(uint8_t sclGPIO) {
    gpio_set_function(sclGPIO, GPIO_FUNC_I2C);
    gpio_pull_up(sclGPIO);
}

static int i2cInternalWriteBlocking(const uint8_t *bytesToSend, uint16_t numberOfBytesToSend,
                                    uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return i2c_write_blocking(i2cHost, slaveAddress, bytesToSend, numberOfBytesToSend, 0);
}

static int i2cInternalReadBlocking(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                   uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return i2c_read_blocking(i2cHost, slaveAddress, responseBuffer, sizeOfResponseBuffer, 0);
}

/* endregion */
