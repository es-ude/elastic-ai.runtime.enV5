#ifndef ENV5_I2C_INTERNAL_HEADER
#define ENV5_I2C_INTERNAL_HEADER

#include <hardware/i2c.h>
#include <stdint.h>

static void i2cInternalSetupPin(uint8_t gpio);

static void i2cInternalSetupScl(uint8_t sclGPIO);

static int i2cInternalWriteBlocking(const uint8_t *bytesToSend, uint16_t numberOfBytesToSend,
                                    uint8_t slaveAddress, i2c_inst_t *hostAddress);

static int i2cInternalReadBlocking(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                   uint8_t slaveAddress, i2c_inst_t *hostAddress);

static bool checkFrequencyInRange(soll, ist);

#endif /* ENV5_I2C_INTERNAL_HEADER */
