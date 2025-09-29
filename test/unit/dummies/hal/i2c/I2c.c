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

i2cErrorCode_t i2cUnittestWriteCommandPass(const uint8_t *commandBuffer,
                                           uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                           i2c_inst_t *i2cHost) {
    return I2C_NO_ERROR;
}

/* endregion I2C_Write DUMMIES */
/* region I2C_Read_Helper */
/*!
 * @brief failing I2C implementation
 *
 * I2C only writes one byte,
 * writing multiple bytes in a row breaks this rule
 */
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

/* endregion I2C_Read DUMMIES */
