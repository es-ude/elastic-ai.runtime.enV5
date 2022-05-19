#include "i2c.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdint.h>

/****************************************************/
/* region STATIC FUNCTION PROTOTYPES */

static void setup_sda(uint8_t sdaGPIO);

static void setup_scl(uint8_t sclGPIO);

static int I2C_write_blocking(const uint8_t *bytesToSend, uint16_t numberOfBytesToSend, uint8_t slaveAddress,
                              i2c_inst_t *i2cHost);

static int I2C_read_blocking(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer, uint8_t slaveAddress,
                             i2c_inst_t *i2cHost);

/* endregion */
/****************************************************/
/* region FUNCTION IMPLEMENTATIONS FROM HEADER FILE*/

void I2C_Init(i2c_inst_t *i2cHost, uint32_t baudRate, uint8_t sdaGPIO, uint8_t sclGPIO) {
    i2c_init(i2cHost, baudRate);
    setup_sda(sdaGPIO);
    setup_scl(sclGPIO);
}

I2C_ErrorCode I2C_WriteCommand(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                               uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    int successfulTransmit = I2C_write_blocking(commandBuffer, sizeOfCommandBuffer, slaveAddress, i2cHost);

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

I2C_ErrorCode I2C_ReadData(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                           i2c_inst_t *i2cHost) {
    int successfulTransmit = I2C_read_blocking(readBuffer, sizeOfReadBuffer, slaveAddress, i2cHost);

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
/****************************************************/
/* region STATIC FUNCTION IMPLEMENTATIONS */

static void setup_sda(uint8_t sdaGPIO) {
    gpio_set_function(sdaGPIO, GPIO_FUNC_I2C);
    gpio_pull_up(sdaGPIO);
}

static void setup_scl(uint8_t sclGPIO) {
    gpio_set_function(sclGPIO, GPIO_FUNC_I2C);
    gpio_pull_up(sclGPIO);
}

static int I2C_write_blocking(const uint8_t *bytesToSend, uint16_t numberOfBytesToSend, uint8_t slaveAddress,
                              i2c_inst_t *i2cHost) {
    return i2c_write_blocking(i2cHost, slaveAddress, bytesToSend, numberOfBytesToSend, 0);
}

static int I2C_read_blocking(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer, uint8_t slaveAddress,
                             i2c_inst_t *i2cHost) {
    return i2c_read_blocking(i2cHost, slaveAddress, responseBuffer, sizeOfResponseBuffer, 0);
}

/* endregion */
