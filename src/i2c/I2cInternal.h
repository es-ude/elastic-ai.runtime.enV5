#ifndef INCLUDE_ENV5_I2C_INTERNAL_HEADER
#warning Internal header file "I2cInternal.h" included from elsewhere
#undef INCLUDE_ENV5_I2C_INTERNAL_HEADER
#endif

#ifndef ENV5_I2C_INTERNAL_HEADER
#define ENV5_I2C_INTERNAL_HEADER

static void i2cInternalSetupSda(uint8_t sdaGPIO);

static void i2cInternalSetupScl(uint8_t sclGPIO);

static int i2cInternalWriteBlocking(const uint8_t *bytesToSend, uint16_t numberOfBytesToSend,
                                    uint8_t slaveAddress, i2c_inst_t *i2cHost);

static int i2cInternalReadBlocking(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                   uint8_t slaveAddress, i2c_inst_t *i2cHost);

#endif /* ENV5_I2C_INTERNAL_HEADER */
