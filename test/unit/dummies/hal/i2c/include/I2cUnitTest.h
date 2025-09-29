#ifndef ENV5_I2C_UNITTEST_HEADER
#define ENV5_I2C_UNITTEST_HEADER

#include "eai/hal/I2c.h"

/* region VARIABLES */

static const uint8_t byteZero = 0xBE;
static const uint8_t byteOne = 0xEF;
static const uint8_t dataFormatFullResON = byteZero;
static const uint8_t dataFormatFullResOFF = byteZero & 11110111;

static const uint8_t correctByteChecksum = 0x92;
static const uint8_t wrongByteChecksum = 0x00;

extern i2cErrorCode_t (*i2cUnittestWriteCommand)(const uint8_t *commandBuffer,
                                                 uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                                 i2c_inst_t *i2cHost);
extern i2cErrorCode_t (*i2cUnittestReadCommand)(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                uint8_t slaveAddress, i2c_inst_t *i2cHost);

/* endregion VARIABLES */

/* region FUNCTIONS */
/* region FUNCTIONS_HELPER */
void i2cUnittestWriteByteMultipleTimes(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                       uint8_t byteToWrite);
/* endregion FUNCTIONS_HELPER */
i2cErrorCode_t i2cUnittestWriteCommandHardwareDefect(const uint8_t *commandBuffer,
                                                     uint16_t sizeOfCommandBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestWriteCommandAckMissing(const uint8_t *commandBuffer,
                                                 uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                                 i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestWriteCommandPass(const uint8_t *commandBuffer,
                                           uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                           i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandHardwareDefect(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandAckMissing(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                uint8_t slaveAddress, i2c_inst_t *i2cHost);

/* endregion FUNCTIONS*/

#endif /* ENV5_I2C_UNITTEST_HEADER */
