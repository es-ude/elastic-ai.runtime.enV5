//
// Created by David P. Federl
//

#include "adxl345b_internal.h"
#include "adxl345b_public.h"
#include "typedefs.h"
#include "i2c.h"
#include "pico/time.h"
#include <stdint.h>


/* region HEADER FUNCTION IMPLEMENTATIONS */

adxl345b_errorCode adxl345b_init ( i2c_inst_t * i2cHost, adxl345b_i2c_slave_address i2cAddress )
  {
    /* save i2c access for later usage */
    i2c_SensorConfiguration.i2c_host          = i2cHost;
    i2c_SensorConfiguration.i2c_slave_address = i2cAddress;
    
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
    
    adxl345b_errorCode errorCode = writeDefaultLowPowerConfiguration ( );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    return ADXL345B_NO_ERROR;
  }

adxl345b_errorCode adxl345b_writeConfigurationToSensor ( adxl345b_register registerToWrite, adxl345b_configuration configuration )
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

adxl345b_errorCode adxl345b_changeMeasurementRange ( adxl345b_range newRange )
  {
    selectedRange = newRange;
    
    /* right adjusted storage, selected range settings for full resolution */
    adxl345b_errorCode errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_DATA_FORMAT, selectedRange.settingForRange );
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
    adxl345b_errorCode errorCode;
    uint8_t            interruptSources;
    uint8_t            sizeOfResponseBuffer = 6;
    uint8_t            responseBuffer[sizeOfResponseBuffer];
    
    /* check if data is ready */
    do
      {
        errorCode = readDataFromSensor ( ADXL345B_REGISTER_INTERRUPT_SOURCE, & interruptSources, 1 );
        if ( errorCode != ADXL345B_NO_ERROR )
          {
            return errorCode;
          }
      } while ( ! ( interruptSources & 0b10000000 ) );
    
    errorCode = readDataFromSensor ( ADXL345B_REGISTER_DATA_X, responseBuffer, sizeOfResponseBuffer );
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

adxl345b_errorCode adxl345b_performSelfTest ( int * delta_x, int * delta_y, int * delta_z )
  {
    adxl345b_errorCode errorCode;
    
    /* standard mode, 100Hz */
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_BW_RATE, 0b00001010 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    /* disable sleep/wakeup functions, start measurements */
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_POWER_CONTROL, 0b00001000 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    /* right adjusted, 16G range */
    errorCode = adxl345b_changeMeasurementRange ( ADXL345B_16G_RANGE );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    sleep_ms ( 2 );
    
    /* collect 100 samples without force */
    int samplesWithoutForce[100][3];
    int counter = 0;
    while ( counter < 100 )
      {
        uint8_t interrupt_source;
        errorCode = readDataFromSensor ( ADXL345B_REGISTER_INTERRUPT_SOURCE, & interrupt_source, 1 );
        if ( errorCode != ADXL345B_NO_ERROR )
          {
            return errorCode;
          }
        
        if ( interrupt_source & 0b10000000 )
          {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];
            errorCode = readDataFromSensor ( ADXL345B_REGISTER_DATA_X, responseBuffer, sizeOfResponseBuffer );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            
            int data_x, data_y, data_z;
            errorCode = convertRawValueToLSB ( & responseBuffer[ 0 ], & data_x );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            errorCode = convertRawValueToLSB ( & responseBuffer[ 2 ], & data_y );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            errorCode = convertRawValueToLSB ( & responseBuffer[ 4 ], & data_z );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            
            samplesWithoutForce[ counter ][ 0 ] = data_x;
            samplesWithoutForce[ counter ][ 1 ] = data_y;
            samplesWithoutForce[ counter ][ 2 ] = data_z;
            
            sleep_ms ( 20 );
            counter ++;
          }
      }
    
    /* calculate average of samples without force */
    int       sum_samplesWithoutForce_x = 0;
    int       sum_samplesWithoutForce_y = 0;
    int       sum_samplesWithoutForce_z = 0;
    for ( int index                     = 0 ; index < 100 ; index ++ )
      {
        sum_samplesWithoutForce_x += samplesWithoutForce[ index ][ 0 ];
        sum_samplesWithoutForce_y += samplesWithoutForce[ index ][ 1 ];
        sum_samplesWithoutForce_z += samplesWithoutForce[ index ][ 2 ];
      }
    int       avg_sampleWithoutForce_x  = sum_samplesWithoutForce_x / 100;
    int       avg_sampleWithoutForce_y  = sum_samplesWithoutForce_y / 100;
    int       avg_sampleWithoutForce_z  = sum_samplesWithoutForce_z / 100;
    
    /* enable self test force */
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_DATA_FORMAT, 0b10001000 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    sleep_ms ( 2 );
    
    /* collect 100 samples with force */
    int samplesWithForce[100][3];
    counter = 0;
    while ( counter < 100 )
      {
        uint8_t interrupt_source;
        errorCode = readDataFromSensor ( ADXL345B_REGISTER_INTERRUPT_SOURCE, & interrupt_source, 1 );
        if ( errorCode != ADXL345B_NO_ERROR )
          {
            return errorCode;
          }
        
        if ( ( interrupt_source & 0b10000000 ) == 0b10000000 )
          {
            uint8_t sizeOfResponseBuffer = 6;
            uint8_t responseBuffer[sizeOfResponseBuffer];
            
            errorCode = readDataFromSensor ( ADXL345B_REGISTER_DATA_X, responseBuffer, sizeOfResponseBuffer );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            
            int data_x, data_y, data_z;
            errorCode = convertRawValueToLSB ( & responseBuffer[ 0 ], & data_x );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            errorCode = convertRawValueToLSB ( & responseBuffer[ 2 ], & data_y );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            errorCode = convertRawValueToLSB ( & responseBuffer[ 4 ], & data_z );
            if ( errorCode != ADXL345B_NO_ERROR )
              {
                return errorCode;
              }
            
            samplesWithForce[ counter ][ 0 ] = data_x;
            samplesWithForce[ counter ][ 1 ] = data_y;
            samplesWithForce[ counter ][ 2 ] = data_z;
            
            sleep_ms ( 20 );
            counter ++;
          }
      }
    
    /* calculate average of samples without force */
    int       sum_samplesWithForce_x = 0;
    int       sum_samplesWithForce_y = 0;
    int       sum_samplesWithForce_z = 0;
    for ( int index                  = 0 ; index < 100 ; index ++ )
      {
        sum_samplesWithForce_x += samplesWithForce[ index ][ 0 ];
        sum_samplesWithForce_y += samplesWithForce[ index ][ 1 ];
        sum_samplesWithForce_z += samplesWithForce[ index ][ 2 ];
      }
    int       avg_sampleWithForce_x  = sum_samplesWithForce_x / 100;
    int       avg_sampleWithForce_y  = sum_samplesWithForce_y / 100;
    int       avg_sampleWithForce_z  = sum_samplesWithForce_z / 100;
    
    // return to default operation mode
    writeDefaultLowPowerConfiguration ( );
    
    /* compare average to datasheet */
    int deltaOfAverage_x = avg_sampleWithForce_x - avg_sampleWithoutForce_x;
    * delta_x = deltaOfAverage_x;
    
    int deltaOfAverage_y = avg_sampleWithForce_y - avg_sampleWithoutForce_y;
    * delta_y = deltaOfAverage_y;
    
    int deltaOfAverage_z = avg_sampleWithForce_z - avg_sampleWithoutForce_z;
    * delta_z = deltaOfAverage_z;
    
    int max_delta_x = 489, max_delta_y = - 46, max_delta_z = 791;
    int min_delta_x = 46, min_delta_y = - 489, min_delta_z = 69;
    if ( ! ( min_delta_x <= deltaOfAverage_x && deltaOfAverage_x <= max_delta_x ) )
      {
        return ADXL345B_SELF_TEST_FAILED_FOR_X;
      }
    
    if ( ! ( min_delta_y <= deltaOfAverage_y && deltaOfAverage_y <= max_delta_y ) )
      {
        return ADXL345B_SELF_TEST_FAILED_FOR_Y;
      }
    
    if ( ! ( min_delta_z <= deltaOfAverage_z && deltaOfAverage_z <= max_delta_z ) )
      {
        return ADXL345B_SELF_TEST_FAILED_FOR_Z;
      }
    
    return ADXL345B_NO_ERROR;
  }

adxl345b_errorCode adxl345b_runSelfCalibration ( )
  {
    int                delta_x, delta_y, delta_z;
    adxl345b_errorCode errorCode = adxl345b_performSelfTest ( & delta_x, & delta_y, & delta_z );
    if ( errorCode == ADXL345B_NO_ERROR )
      {
        return ADXL345B_NO_ERROR;
      }
    if ( errorCode != ADXL345B_SELF_TEST_FAILED_FOR_X && errorCode != ADXL345B_SELF_TEST_FAILED_FOR_Y && errorCode != ADXL345B_SELF_TEST_FAILED_FOR_Z )
      {
        return errorCode;
      }
    
    int8_t offset_x, offset_y, offset_z;
    int    max_delta_x = 489, max_delta_y = - 46, max_delta_z = 791;
    int    min_delta_x = 46, min_delta_y = - 489, min_delta_z = 69;
    offset_x = calculateCalibrationOffset ( delta_x, max_delta_x, min_delta_x );
    offset_y = calculateCalibrationOffset ( delta_y, max_delta_y, min_delta_y );
    offset_z = calculateCalibrationOffset ( delta_z, max_delta_z, min_delta_z );
    
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_OFFSET_X, offset_x );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_OFFSET_Y, offset_y );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_OFFSET_Z, offset_z );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    return ADXL345B_NO_ERROR;
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

static adxl345b_errorCode convertRawValueToLSB ( const uint8_t rawData[2], int * lsbValue )
  {
    if ( rawData[ 1 ] <= ( selectedRange.msbMask >> 1 ) )
      {
        /* CASE: positive value */
        uint16_t rawValue = ( ( uint16_t ) ( rawData[ 1 ] & ( selectedRange.msbMask >> 1 ) ) << 8 ) | ( uint16_t ) rawData[ 0 ];
        * lsbValue = ( int ) rawValue;
      }
    else
      {
        /* CASE: negative value => manual translation needed
         *
         * 1. revert two complement: number minus 1 and flip bits
         * 2. convert to int and multiply with -1
         */
        uint16_t rawValue = ( ( uint16_t ) ( rawData[ 1 ] & ( selectedRange.msbMask >> 1 ) ) << 8 ) | ( uint16_t ) rawData[ 0 ];
        rawValue = ( rawValue - 0x0001 ) ^ ( ( selectedRange.msbMask >> 1 ) << 8 | 0x00FF );
        * lsbValue = ( - 1 ) * ( int ) rawValue;
      }
    
    return ADXL345B_NO_ERROR;
  }

static adxl345b_errorCode convertLSBtoGValue ( int lsb, float * gValue )
  {
    float realValue = ( float ) lsb * selectedRange.scaleFactor;
    * gValue = realValue;
    
    return ADXL345B_NO_ERROR;
  }

static adxl345b_errorCode convertRawValueToGValue ( const uint8_t rawData[2], float * gValue )
  {
    int                intermediate_lsb;
    adxl345b_errorCode errorCode = convertRawValueToLSB ( rawData, & intermediate_lsb );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    float intermediate_gValue;
    errorCode = convertLSBtoGValue ( intermediate_lsb, & intermediate_gValue );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    * gValue = intermediate_gValue;
    return ADXL345B_NO_ERROR;
  }

static adxl345b_errorCode writeDefaultLowPowerConfiguration ( )
  {
    /* enable low power mode at 12.5Hz data output rate */
    adxl345b_errorCode errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_BW_RATE, 0b00010111 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    /* disable auto sleep, enable normal operation mode */
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_POWER_CONTROL, 0b00001000 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    /* disable all interrupts */
    errorCode = adxl345b_writeConfigurationToSensor ( ADXL345B_REGISTER_INTERRUPT_ENABLE, 0b00000000 );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    /* right adjusted storage, enable 10 bit 2G resolution */
    errorCode = adxl345b_changeMeasurementRange ( ADXL345B_2G_RANGE );
    if ( errorCode != ADXL345B_NO_ERROR )
      {
        return errorCode;
      }
    
    return ADXL345B_NO_ERROR;
  }

static int8_t calculateCalibrationOffset ( int measuredDelta, int maxValue, int minValue )
  {
    if ( measuredDelta <= minValue )
      {
        return ( int8_t ) ( minValue - measuredDelta );
      }
    if ( measuredDelta >= maxValue )
      {
        return ( int8_t ) ( maxValue - measuredDelta );
      }
    return 0;
  }

/* endregion */
