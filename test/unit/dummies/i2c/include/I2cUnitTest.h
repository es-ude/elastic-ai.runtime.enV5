#ifndef ENV5_I2C_UNITTEST_HEADER
#define ENV5_I2C_UNITTEST_HEADER

#include "I2c.h"

/* region VARIABLES */

static const uint8_t byteZero = 0xBE;
static const uint8_t byteOne = 0xEF;
static const uint8_t correctByteChecksum = 0x92;
static const uint8_t wrongByteChecksum = 0x00;

extern i2cErrorCode_t (*i2cUnittestWriteCommand)(const uint8_t *commandBuffer,
                                                 uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                                 i2c_inst_t *i2cHost);
extern i2cErrorCode_t (*i2cUnittestReadCommand)(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                uint8_t slaveAddress, i2c_inst_t *i2cHos);

/* endregion */

/* region FUNCTIONS */

i2cErrorCode_t i2cUnittestWriteCommandHardwareDefect(const uint8_t *commandBuffer,
                                                     uint16_t sizeOfCommandBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestWriteCommandAckMissing(const uint8_t *commandBuffer,
                                                 uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                                 i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestWriteCommandPassForSht3x(const uint8_t *commandBuffer,
                                                   uint16_t sizeOfCommandBuffer,
                                                   uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestWriteCommandPassForAdxl345b(const uint8_t *commandBuffer,
                                                      uint16_t sizeOfCommandBuffer,
                                                      uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestWriteCommandPassForPac193x(const uint8_t *commandBuffer,
                                                     uint16_t sizeOfCommandBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandHardwareDefect(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandAckMissing(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassForSht3x(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                  uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandProvokeChecksumFailForSht3x(uint8_t *readBuffer,
                                                                 uint8_t sizeOfReadBuffer,
                                                                 uint8_t slaveAddress,
                                                                 i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345b(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassForPac193x(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost);

/* endregion */

#endif /* ENV5_I2C_UNITTEST_HEADER */
