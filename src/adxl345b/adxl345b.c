//
// Created by David P. Federl
//

#include "adxl345b_internal.h"
#include "adxl345b_public.h"
#include "typedefs.h"
#include "i2c.h"
#include "pico/time.h"
#include <stdint.h>


/* region VARIABLES */

static adxl345b_i2cSensorConfiguration i2c_SensorConfiguration; /*!< i2c configuration for the sensor */

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

adxl345b_errorCode adxl345b_init ( i2c_inst_t * i2cHost )
  {
    /* save i2c access for later usage */
    i2c_SensorConfiguration.i2c_host          = i2cHost;
    i2c_SensorConfiguration.i2c_slave_address = 0x53;
    
    I2C_Init ( i2c_SensorConfiguration.i2c_host, ( 100 * 1000 ), 0, 1 );
    
    /* sleep to make sure the sensor is fully initialized */
    sleep_ms ( 2 );
    
    /* Check if sensor ADXL345B is on Bus by requesting serial number without processing */
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[ 0 ] = ADXL345B_REGISTER_DEVICE_ID;
    
    /* if i2c returns error -> sensor not available on bus */
    I2C_ErrorCode I2C_errorCode = I2C_WriteCommand ( commandBuffer,
                                                     sizeOfCommandBuffer,
                                                     i2c_SensorConfiguration.i2c_slave_address,
                                                     i2c_SensorConfiguration.i2c_host
                                                   );
    if ( I2C_errorCode != I2C_NO_ERROR )
      {
        return ADXL345B_INIT_ERROR;
      }
    
    /* enable low power mode at 12.5Hz data output rate */
    adxl345b_errorCode errorCode = writeConfigurationToSensor ( ADXL345B_REGISTER_BW_RATE, 0b00010111 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    /* disable auto sleep, enable normal operation mode */
    errorCode = writeConfigurationToSensor ( ADXL345B_REGISTER_POWER_CONTROL, 0b00001000 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    /* disable all interrupts */
    errorCode = writeConfigurationToSensor ( ADXL345B_REGISTER_INTERRUPT_ENABLE, 0b00000000 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    /* right adjusted storage, enable full resolution for selected range */
    errorCode = writeConfigurationToSensor ( ADXL345B_REGISTER_DATA_FORMAT, MEASUREMENTS_RANGE_SETTINGS );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    return ADXL345B_NO_ERROR;
  }

adxl345b_errorCode adxl345b_readSerialNumber ( uint8_t * serialNumber )
  {
    uint8_t sizeOfResponseBuffer = 1;
    uint8_t responseBuffer[sizeOfResponseBuffer];
    
    adxl345b_errorCode adxl345bErrorCode = readDataFromSensor ( ADXL345B_REGISTER_DEVICE_ID, responseBuffer, sizeOfResponseBuffer );
    if ( adxl345bErrorCode != ADXL345B_NO_ERROR )    /* if i2c returns error -> sensor not available on bus */
      {
        return adxl345bErrorCode;
      }
    
    * serialNumber = responseBuffer[ 0 ];
    
    return ADXL345B_NO_ERROR;
  }

adxl345b_errorCode adxl345b_readMeasurements ( float * xAxis, float * yAxis, float * zAxis )
  {
    uint8_t sizeOfResponseBuffer = 6;
    uint8_t responseBuffer[sizeOfResponseBuffer];
    
    adxl345b_errorCode errorCode = readDataFromSensor ( ADXL345B_REGISTER_DATA_X, responseBuffer, sizeOfResponseBuffer );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    errorCode = convertRawValueToGValue ( & responseBuffer[ 0 ], xAxis );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    errorCode = convertRawValueToGValue ( & responseBuffer[ 2 ], yAxis );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    errorCode = convertRawValueToGValue ( & responseBuffer[ 4 ], zAxis );
    
    return errorCode;
  }

adxl345b_errorCode adxl345b_performSelfTest ( )
  {
    // TODO: Implement self test
    return ADXL345B_CONFIGURATION_ERROR;
  }

/* endregion */

/* region HELPER FUNCTION IMPLEMENTATIONS */

static adxl345b_errorCode readDataFromSensor ( adxl345b_register registerToRead, uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer )
  {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[ 0 ] = registerToRead;
    
    I2C_ErrorCode errorCode = I2C_WriteCommand ( commandBuffer, sizeOfCommandBuffer, i2c_SensorConfiguration.i2c_slave_address, i2c_SensorConfiguration.i2c_host
                                               );
    if ( errorCode != I2C_NO_ERROR )
      {
        return ADXL345B_SEND_COMMAND_ERROR;
      }
    
    errorCode = I2C_ReadData ( responseBuffer, sizeOfResponseBuffer, i2c_SensorConfiguration.i2c_slave_address, i2c_SensorConfiguration.i2c_host );
    if ( errorCode != I2C_NO_ERROR )
      {
        return ADXL345B_RECEIVE_DATA_ERROR;
      }
    
    return ADXL345B_NO_ERROR;
  }

static adxl345b_errorCode writeConfigurationToSensor ( adxl345b_register registerToWrite, uint8_t configuration )
  {
    uint8_t sizeOfCommandBuffer = 2;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[ 0 ] = registerToWrite;
    commandBuffer[ 1 ] = configuration;
    
    I2C_ErrorCode errorCode = I2C_WriteCommand ( commandBuffer, sizeOfCommandBuffer, i2c_SensorConfiguration.i2c_slave_address, i2c_SensorConfiguration.i2c_host
                                               );
    if ( errorCode != I2C_NO_ERROR )
      {
        return ADXL345B_CONFIGURATION_ERROR;
      }
    
    return ADXL345B_NO_ERROR;
  }

static adxl345b_errorCode convertRawValueToGValue ( const uint8_t rawData[2], float * gValue )
  {
    if ( rawData[ 1 ] <= ( MSB_MASK >> 1 ) )
      {
        /* CASE: positive value */
        int16_t rawValue  = ( int16_t ) ( ( ( uint16_t ) ( rawData[ 1 ] & MSB_MASK ) << 8 ) | ( uint16_t ) rawData[ 0 ] );
        float   realValue = ( float ) rawValue * SCALE_FACTOR_FOR_RANGE;
        * gValue = realValue;
      }
    else
      {
        /* CASE: negative value => manual translation needed
         *
         * 1. revert 10 bit two complement
         * -> number-1 and Flip least 9 bits
         * 2. convert to float value
         * 3. multiply with (-1)
         * 4. multiply with scale factor
         */
        uint16_t rawValue = ( ( uint16_t ) ( rawData[ 1 ] & ( MSB_MASK >> 1 ) ) << 8 ) | ( uint16_t ) rawData[ 0 ];
        rawValue = ( rawValue - 0x0001 ) ^ 0x01FF;
        float realValue = ( - 1 ) * ( float ) rawValue * SCALE_FACTOR_FOR_RANGE;
        * gValue = realValue;
      }
    return ADXL345B_NO_ERROR;
  }

/* endregion */
