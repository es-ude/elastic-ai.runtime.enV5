#include "I2c.h"
#include "Gpio.h"
#include "I2cInternal.h"


#include <hardware/i2c.h>

/* region FUNCTION IMPLEMENTATIONS FROM HEADER FILE*/


void i2cInit(i2cConfig_t *i2cConfig) {
    i2c_init(i2cConfig->i2cInstance, i2cConfig->frequency);
    i2cInternalSetupPin(i2cConfig->sdaPin);
    i2cInternalSetupPin(i2cConfig->sclPin);
}

i2cErrorCode_t i2cWriteCommand(i2cConfig_t *i2cConfig, uint8_t slaveAddress,
                               const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer) {
    int successfulTransmit =
        i2cInternalWriteBlocking(commandBuffer, sizeOfCommandBuffer, slaveAddress, i2cConfig->i2cInstance);

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

i2cErrorCode_t i2cReadData(i2cConfig_t *i2cConfig, uint8_t slaveAddress, uint8_t *readBuffer,
                           uint8_t sizeOfReadBuffer) {
    int successfulTransmit =
        i2cInternalReadBlocking(readBuffer, sizeOfReadBuffer, slaveAddress, i2cConfig->i2cInstance);

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


static void i2cInternalSetupPin(uint8_t gpio) {
    gpioSetPinFunction(gpio, GPIO_FUNCTION_I2C);
    gpioEnablePullUp(gpio);
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
