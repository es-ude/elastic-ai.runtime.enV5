#include "I2c.h"
#include "Gpio.h"
#include "I2cInternal.h"
#include "Common.h"

#include <hardware/i2c.h>

/* region FUNCTION IMPLEMENTATIONS FROM HEADER FILE*/

i2cErrorCode_t i2cInit(i2cConfiguration_t *i2cConfiguration) {
    uint32_t actualBaudrate = i2c_init(i2cConfiguration->i2cInstance, i2cConfiguration->frequency);
    if(!i2cInternalCheckFrequencyInRange(actualBaudrate, i2cConfiguration->frequency)){return I2C_FREQUENCY_ERROR;}
    i2cInternalSetupPin(i2cConfiguration->sdaPin);
    i2cInternalSetupPin(i2cConfiguration->sclPin);
    return I2C_NO_ERROR;
}

i2cErrorCode_t i2cWriteCommand(i2c_inst_t *hostAddress, uint8_t slaveAddress,
                               const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer) {
    int successfulTransmit =
        i2cInternalWriteBlocking(commandBuffer, sizeOfCommandBuffer, slaveAddress, hostAddress);

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

i2cErrorCode_t i2cReadData(i2c_inst_t *hostAddress, uint8_t slaveAddress, uint8_t *readBuffer,
                           uint8_t sizeOfReadBuffer) {
    int successfulTransmit =
        i2cInternalReadBlocking(readBuffer, sizeOfReadBuffer, slaveAddress, hostAddress);

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

static bool i2cInternalCheckFrequencyInRange(uint32_t actualFrequency, uint32_t targetFrequency){
    uint16_t delta = 1000;
    if(actualFrequency + delta < targetFrequency){return false;}
    else if (actualFrequency > targetFrequency){
            PRINT("Warning: actualFrequency above targetFrequency");
            return false;}
    return true;
}

static void i2cInternalSetupPin(uint8_t gpio) {
    gpioSetPinFunction(gpio, GPIO_FUNCTION_I2C);
    gpioEnablePullUp(gpio);
}

static int i2cInternalWriteBlocking(const uint8_t *bytesToSend, uint16_t numberOfBytesToSend,
                                    uint8_t slaveAddress, i2c_inst_t *hostAddress) {
    return i2c_write_blocking(hostAddress, slaveAddress, bytesToSend, numberOfBytesToSend, 0);
}

static int i2cInternalReadBlocking(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                   uint8_t slaveAddress, i2c_inst_t *hostAddress) {
    return i2c_read_blocking(hostAddress, slaveAddress, responseBuffer, sizeOfResponseBuffer, 0);
}

/* endregion */
