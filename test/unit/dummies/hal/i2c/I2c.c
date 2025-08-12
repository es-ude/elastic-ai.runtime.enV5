#include "eai/hal/I2c.h"
#include "I2cUnitTest.h"

#include <stdint.h>

/* region i2cInit DUMMY */

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost
 * @param i2cConfiguration
 */

i2cErrorCode_t i2cInit(i2cConfiguration_t *i2cConfiguration) {
    return 0;
}

/* endregion I2cInit DUMMY*/

/* region I2C_Write DUMMIES */

i2cErrorCode_t (*i2cUnittestWriteCommand)(const uint8_t *commandBuffer,
                                          uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                          i2c_inst_t *i2cHost);

i2cErrorCode_t i2cWriteCommand(i2c_inst_t *hostAddress, uint8_t slaveAddress,
                               const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer) {
    return i2cUnittestWriteCommand(commandBuffer, sizeOfCommandBuffer, slaveAddress, hostAddress);
}

i2cErrorCode_t i2cUnittestWriteCommandHardwareDefect(const uint8_t *commandBuffer,
                                                     uint16_t sizeOfCommandBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_INIT_ERROR;
}

i2cErrorCode_t i2cUnittestWriteCommandAckMissing(const uint8_t *commandBuffer,
                                                 uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                                 i2c_inst_t *i2cHost) {
    return I2C_ACK_ERROR;
}

i2cErrorCode_t i2cUnittestWriteCommandPassForSht3x(const uint8_t *commandBuffer,
                                                   uint16_t sizeOfCommandBuffer,
                                                   uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_NO_ERROR;
}

i2cErrorCode_t i2cUnittestWriteCommandPassForAdxl345b(const uint8_t *commandBuffer,
                                                      uint16_t sizeOfCommandBuffer,
                                                      uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_NO_ERROR;
}

i2cErrorCode_t i2cUnittestWriteCommandPassForPac193x(const uint8_t *commandBuffer,
                                                     uint16_t sizeOfCommandBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_NO_ERROR;
}

/* endregion I2C_Write DUMMIES */

/* region I2C_Read_Helper */
void i2cUnittestWriteByteMultipleTimes(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                       uint8_t byteToWrite) {
    /* generate sample data without any real world connection to test
     * implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index++) {
        readBuffer[index] = byteToWrite;
    }
}
/* endregion I2C_Read_Helper */

/* region I2C_Read DUMMIES */

i2cErrorCode_t (*i2cUnittestReadCommand)(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                         uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cReadData(i2c_inst_t *hostAddress, uint8_t slaveAddress, uint8_t *readBuffer,
                           uint8_t sizeOfReadBuffer) {
    return i2cUnittestReadCommand(readBuffer, sizeOfReadBuffer, slaveAddress, hostAddress);
}

i2cErrorCode_t i2cUnittestReadCommandHardwareDefect(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_INIT_ERROR;
}

i2cErrorCode_t i2cUnittestReadCommandAckMissing(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_ACK_ERROR;
}

i2cErrorCode_t i2cUnittestReadCommandPassForSht3x(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                  uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    /**
     * @brief failing I²C implementation
     *
     * I2C only writes one byte,
     * this implementation breaks this rule by always writing three bytes in a
     * row
     *
     * FIXME: Improve implementation to match I2C specifications
     */

    /* generate sample data without any real world connection to test
     * implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index = index + 3) {
        readBuffer[index] = byteZero;
        readBuffer[index + 1] = byteOne;
        readBuffer[index + 2] = correctByteChecksum;
    }

    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandProvokeChecksumFailForSht3x(uint8_t *readBuffer,
                                                                 uint8_t sizeOfReadBuffer,
                                                                 uint8_t slaveAddress,
                                                                 i2c_inst_t *i2cHost) {
    /**
     * @brief failing I2C implementation
     *
     * I2C only writes one byte,
     * this implementation breaks this rule by always writing three bytes in a
     * row
     *
     * FIXME: Improve implementation to match I2C specifications
     */

    /* generate sample data without any real world connection to test
     * implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index = index + 3) {
        readBuffer[index] = byteZero;
        readBuffer[index + 1] = byteOne;
        readBuffer[index + 2] = wrongByteChecksum;
    }

    return 0x00;
}

/* region Adxl345b */

i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bInBypassMode(uint8_t *readBuffer,
                                                                 uint8_t sizeOfReadBuffer,
                                                                 uint8_t slaveAddress,
                                                                 i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);

    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bInStreamMode(uint8_t *readBuffer,
                                                                 uint8_t sizeOfReadBuffer,
                                                                 uint8_t slaveAddress,
                                                                 i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    if (sizeOfReadBuffer == 1) {
        readBuffer[0] = 0b10000000 | 0b00000010 |
                        0b00011111; // set Stream Mode, Watermark and samples for Trigger
        return 0x00;
    }
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteOne);
    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bInFifoMode(uint8_t *readBuffer,
                                                               uint8_t sizeOfReadBuffer,
                                                               uint8_t slaveAddress,
                                                               i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    if (sizeOfReadBuffer == 1) {
        readBuffer[0] = 0b01000000 | 0b00000010 |
                        0b00011111; // set FiFo Mode, Watermark and samples for Trigger
        return 0x00;
    }
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);
    return 0x00;
}
i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bInTriggerMode(uint8_t *readBuffer,
                                                                  uint8_t sizeOfReadBuffer,
                                                                  uint8_t slaveAddress,
                                                                  i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    if (sizeOfReadBuffer == 1) {
        readBuffer[0] = 0b11000000 | 0b00000010 |
                        0b00011111; // set Trigger Mode, Watermark and samples for Trigger
        return 0x00;
    }
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);
    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bWithFullResOFF(uint8_t *readBuffer,
                                                                   uint8_t sizeOfReadBuffer,
                                                                   uint8_t slaveAddress,
                                                                   i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, dataFormatFullResOFF);

    return 0x00;
}
i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bWithFullResON(uint8_t *readBuffer,
                                                                  uint8_t sizeOfReadBuffer,
                                                                  uint8_t slaveAddress,
                                                                  i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, dataFormatFullResON);

    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bWithFullResOFF(uint8_t *readBuffer,
                                                                   uint8_t sizeOfReadBuffer,
                                                                   uint8_t slaveAddress,
                                                                   i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, dataFormatFullResOFF);

    return 0x00;
}
i2cErrorCode_t i2cUnittestReadCommandPassForAdxl345bWithFullResON(uint8_t *readBuffer,
                                                                  uint8_t sizeOfReadBuffer,
                                                                  uint8_t slaveAddress,
                                                                  i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, dataFormatFullResON);

    return 0x00;
}

/* endregion Adxl345b */

i2cErrorCode_t i2cUnittestReadCommandPassForPac193x(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);

    return 0x00;
}

/* endregion I2C_Read DUMMIES */
