#ifndef ENV5_I2C_HEADER
#define ENV5_I2C_HEADER

#include "I2cTypedefs.h"
#include <stdint.h>


/*!
 * @brief initializes the ports for the I2C interfaces
 * @param i2cConfig[I2CDevice] struct that contains the I2C configuration which can be found in HwConfig.h
 */
void i2cInit(i2cConfig_t *i2cConfig);

/*!
 * @brief sends a byte array to the slave
 * @param commandBuffer uint8_t array that holds the commands to be send to slave
 * @param slaveAddress address of the I2C slave (Sensor)
 * @param i2cConfig I2C Interface (i2c0 or i2c1)
 */
i2cErrorCode_t i2cWriteCommand(i2cConfig_t *i2cConfig, uint8_t slaveAddress,
                               const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer);

/*!
 * @brief reads bytes from slave
 * @param readBuffer:  pointer to uint8_t array that stores the received data
 * @param slaveAddress:  address of the I2C slave (Sensor)
 * @param i2cConfig:  I2C Interface (i2c0 or i2c1)
 */
i2cErrorCode_t i2cReadData(i2cConfig_t *i2cConfig, uint8_t slaveAddress, uint8_t *readBuffer,
                           uint8_t sizeOfReadBuffer);

#endif /* ENV5_I2C_HEADER */
