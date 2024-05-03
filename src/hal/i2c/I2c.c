#include "I2c.h"
#include "Gpio.h"
#include "I2cInternal.h"

#include <hardware/i2c.h>
#include "Common.h"
/* region FUNCTION IMPLEMENTATIONS FROM HEADER FILE*/


i2cErrorCode_t i2cInit(i2cConfiguration_t *i2cConfiguration) {
    uint32_t actualBaudrate = i2c_init(i2cConfiguration->i2cInstance, i2cConfiguration->frequency);
    //TODO: wird nie exakt stimmen für Überprüfung
    //ich brauche noch ein !!!delta!!!
    PRINT_DEBUG("Baudrate set to %u", actualBaudrate);
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

/*
 * TODO: static bool checkFrequencyInRange(soll, ist){
    vergleich und bool zurück geben
}*/

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
