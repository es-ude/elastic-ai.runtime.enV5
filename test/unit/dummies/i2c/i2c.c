#include "i2c_test.h"
#include "i2c.h"
#include <stdint.h>

/* region I2C_Init DUMMY */

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost
 * @param baudrate
 * @param sdaGPIO
 * @param sclGPIO
 */
void I2C_Init(i2c_inst_t *i2cHost, uint32_t baudrate, uint8_t sdaGPIO, uint8_t sclGPIO) {

}

/* endregion */

/* region I2C_Write DUMMIES */

I2C_ErrorCode (*I2C_WriteCommand_ptr)(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                      i2c_inst_t *i2cHost);


I2C_ErrorCode I2C_WriteCommand(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                               i2c_inst_t *i2cHost) {
    return I2C_WriteCommand_ptr(commandBuffer, sizeOfCommandBuffer, slaveAddress, i2cHost);
}

I2C_ErrorCode
I2C_WriteCommand_Hardware_defect(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                 i2c_inst_t *i2cHost
) {
    return I2C_INIT_ERROR;
}

I2C_ErrorCode
I2C_WriteCommand_ACK_missing(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                             i2c_inst_t *i2cHost) {
    return I2C_ACK_ERROR;
}

I2C_ErrorCode
I2C_WriteCommand_Pass_for_SHT3X(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                i2c_inst_t *i2cHost) {
    return I2C_NO_ERROR;
}

I2C_ErrorCode I2C_WriteCommand_Pass_for_ADXL345B ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost )
  {
    return I2C_NO_ERROR;
  }

I2C_ErrorCode I2C_WriteCommand_Pass_for_PAC193X ( const uint8_t * commandBuffer, uint16_t sizeOfCommandBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost )
  {
    return I2C_NO_ERROR;
  }

/* endregion */

/* region I2C_Read DUMMIES */

I2C_ErrorCode
(*I2C_ReadCommand_ptr)(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t *i2cHos);

I2C_ErrorCode I2C_ReadData(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_ReadCommand_ptr(readBuffer, sizeOfReadBuffer, slaveAddress, i2cHost);
}

I2C_ErrorCode I2C_ReadCommand_Hardware_defect(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                                              i2c_inst_t *i2cHost) {
    return I2C_INIT_ERROR;
}

I2C_ErrorCode
I2C_ReadCommand_ACK_missing(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_ACK_ERROR;
}

I2C_ErrorCode I2C_ReadCommand_Pass_for_SHT3X(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                                             i2c_inst_t *i2cHost) {
    /**
     * @brief failing I²C implementation
     *
     * I2C only writes one byte,
     * this implementation breaks this rule by always writing three bytes in a row
     *
     * FIXME: Improve implementation to match I2C specifications
     */

    /* generate sample data without any real world connection to test implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index = index + 3) {
        readBuffer[index] = byteZero;
        readBuffer[index + 1] = byteOne;
        readBuffer[index + 2] = correctByteChecksum;
    }

    return 0x00;
}

I2C_ErrorCode
I2C_ReadCommand_provoke_checksum_fail_for_SHT3X(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                                                i2c_inst_t *i2cHost) {
    /**
     * @brief failing I2C implementation
     *
     * I2C only writes one byte,
     * this implementation breaks this rule by always writing three bytes in a row
     *
     * FIXME: Improve implementation to match I2C specifications
     */

    /* generate sample data without any real world connection to test implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index = index + 3) {
        readBuffer[index] = byteZero;
        readBuffer[index + 1] = byteOne;
        readBuffer[index + 2] = wrongByteChecksum;
    }

    return 0x00;
}

I2C_ErrorCode I2C_ReadCommand_Pass_for_ADXL345B(uint8_t *readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress,
                                                i2c_inst_t *i2cHost) {
    /* generate sample data without any real world connection to test implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index++) {
        readBuffer[index] = byteZero;
    }

    return 0x00;
}

I2C_ErrorCode I2C_ReadCommand_Pass_for_PAC193X ( uint8_t * readBuffer, uint8_t sizeOfReadBuffer, uint8_t slaveAddress, i2c_inst_t * i2cHost )
  {
    /* generate sample data without any real world connection to test implementation */
    for ( uint8_t index = 0 ; index < sizeOfReadBuffer ; index ++ )
      {
        readBuffer[ index ] = byteZero;
      }
    
    return 0x00;
  }
  
/* endregion */
  