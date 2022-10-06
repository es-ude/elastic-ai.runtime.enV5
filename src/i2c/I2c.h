#ifndef ENV5_I2C_HEADER
#define ENV5_I2C_HEADER

#include "I2cTypedefs.h"
#include <stdint.h>

/*! \fn i2cInit
 *
 * initializes the ports for the I2C interfaces
 *
 * @param i2cHost:  I2C Interface (i2c0 or i2c1)
 * @param baudRate:  max Baud rate for I2C interface
 * @param sdaGPIO:  GPIO for SDA line
 * @param sclGPIO:  GPIO for SCL line
 */
void i2cInit(i2c_inst_t *i2cHost, uint32_t baudRate, uint8_t sdaGPIO, uint8_t sclGPIO);

/*! \fn I2C_WriteByte
 *
 * @param commandBuffer:  uint8_t array that holds the commands to be send to
 * slave
 * @param slaveAddress:  address of the I2C slave (Sensor)
 * @param i2cHost:  I2C Interface (i2c0 or i2c1)
 */
i2cErrorCode_t i2cWriteCommand(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                               uint8_t slaveAddress, i2c_inst_t *i2cHost);

/*! \fn i2cReadData
 *
 * @param readBuffer:  pointer to uint8_t array that stores the received data
 * @param slaveAddress:  address of the I2C slave (Sensor)
 * @param i2cHost:  I2C Interface (i2c0 or i2c1)
 */
i2cErrorCode_t i2cReadData(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                           i2c_inst_t *i2cHost);

#endif /* ENV5_I2C_HEADER */
