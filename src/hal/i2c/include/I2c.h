#ifndef ENV5_I2C_HEADER
#define ENV5_I2C_HEADER

#include "I2cTypedefs.h"
#include <stdint.h>

/*!
 * @brief initializes the ports for the I2C interfaces
 * @param i2cConfiguration[I2CDevice] struct that contains the I2C configuration which can be found in HwConfig.h
 */
i2cErrorCode_t i2cInit(i2cConfiguration_t *i2cConfiguration);

/*!
 * @brief sends a byte array to the slave
 * @param i2cHost I2C Interface (i2c0 or i2c1)
 * @param slaveAddress address of the I2C slave (Sensor)
 * @param commandBuffer uint8_t array that holds the commands to be send to slave
 */
i2cErrorCode_t i2cWriteCommand(i2c_inst_t *hostAddress, uint8_t slaveAddress,
                               const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer);

/*!
 * @brief reads bytes from slave
 * @param i2cHost I2C Interface (i2c0 or i2c1)
 * @param slaveAddress address of the I2C slave (Sensor)
 * @param readBuffer:  pointer to uint8_t array that stores the received data
 */
i2cErrorCode_t i2cReadData(i2c_inst_t *hostAddress, uint8_t slaveAddress, uint8_t *readBuffer,
                           uint8_t sizeOfReadBuffer);

#endif /* ENV5_I2C_HEADER */
