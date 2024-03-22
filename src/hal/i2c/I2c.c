#include "I2c.h"
#include "Gpio.h"
#include "I2cInternal.h"

#include <hardware/i2c.h>





#include "Common.h"

/* region FUNCTION IMPLEMENTATIONS FROM HEADER FILE*/

void i2cInit( i2c_inst_t *i2cHost, uint32_t baudrate, uint8_t sdaGPIO, uint8_t sclGPIO ) {
    uint actualBaudrate = i2c_init( i2cHost, baudrate );
    PRINT_DEBUG("Baudrate set to %u", actualBaudrate);
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
    gpioSetPinFunction(sdaGPIO, GPIO_FUNCTION_I2C);
    gpioEnablePullUp(sdaGPIO);
}

static void i2cInternalSetupScl(uint8_t sclGPIO) {
    gpioSetPinFunction(sclGPIO, GPIO_FUNCTION_I2C);
    gpioEnablePullUp(sclGPIO);
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
