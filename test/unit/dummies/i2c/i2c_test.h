//
// Created by federl_david on 20.04.22.
//

#ifndef ENV5_I2C_TEST_HEADER
#define ENV5_I2C_TEST_HEADER

#include "i2c.h"

/* region VARIABLES */

static const uint8_t byteZero            = 0xBE;
static const uint8_t byteOne             = 0xEF;
static const uint8_t correctByteChecksum = 0x92;
static const uint8_t wrongByteChecksum   = 0x00;

extern I2C_ErrorCode (* I2C_WriteCommand_ptr) ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );
extern I2C_ErrorCode (* I2C_ReadCommand_ptr) ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHos );

/* endregion */

/* region FUNCTIONS */

I2C_ErrorCode I2C_WriteCommand_Hardware_defect ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost
                                               );

I2C_ErrorCode I2C_WriteCommand_ACK_missing ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_WriteCommand_Pass_for_SHT3X ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_WriteCommand_Pass_for_ADXL345B ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_ReadCommand_Hardware_defect ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_ReadCommand_ACK_missing ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_ReadCommand_Pass_for_SHT3X ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_ReadCommand_provoke_checksum_fail_for_SHT3X ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

I2C_ErrorCode I2C_ReadCommand_Pass_for_ADXL345B ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost );

/* endregion */

#endif /* ENV5_I2C_TEST_HEADER */
