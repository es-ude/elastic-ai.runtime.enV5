//
// Created by David P. Federl
//

#include "i2c/i2c_test.h"
#include "pac193x_public.h"
#include "pac193x_internal.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>


static float                R_SENSE[4]       = { 0, 0.82f, 0, 0 }; /* External resistor: 100kOhm and 3.3V -> 33A */
static pac193x_usedChannels USED_CHANNELS    = { .uint_channelsInUse = 0b00000010 };
static uint8_t              usedChannelIndex = 1;

void setUp ( void )
  {
    /* Default: Point to Pass */
    I2C_WriteCommand_ptr = I2C_WriteCommand_Pass_for_PAC193X;
    I2C_ReadCommand_ptr  = I2C_ReadCommand_Pass_for_PAC193X;
    
    pac193x_setResistanceAtSense ( R_SENSE );
    pac193x_setChannelsInUse ( USED_CHANNELS );
  }

void tearDown ( void )
  {
  }


/* region pac193x_GetSensorInfo */

void pac193x_GetSensorInfo_return_SEND_COMMAND_ERROR_if_hardware_fails ( void )
  {
    pac193x_info info;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo ( & info );
    TEST_ASSERT_EQUAL_UINT8( PAC193X_SEND_COMMAND_ERROR, errorCode );
  }

void pac193x_GetSensorInfo_return_SEND_COMMAND_ERROR_if_ACK_missing ( void )
  {
    pac193x_info info;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo ( & info );
    TEST_ASSERT_EQUAL_UINT8( PAC193X_SEND_COMMAND_ERROR, errorCode );
  }

void pac193x_GetSensorInfo_return_RECEIVE_DATA_ERROR_if_hardware_fails ( void )
  {
    pac193x_info info;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo ( & info );
    TEST_ASSERT_EQUAL_UINT8( PAC193X_RECEIVE_DATA_ERROR, errorCode );
  }

void pac193x_GetSensorInfo_return_RECEIVE_DATA_ERROR_if_ACK_missing ( void )
  {
    pac193x_info info;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo ( & info );
    TEST_ASSERT_EQUAL_UINT8( PAC193X_RECEIVE_DATA_ERROR, errorCode );
  }

void pac193x_GetSensorInfo_read_successful ( void )
  {
    pac193x_info info;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo ( & info );
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetSensorInfo_read_correct_value ( void )
  {
    pac193x_info expectedInfo, actualInfo;
    
    expectedInfo.product_id      = byteZero;
    expectedInfo.manufacturer_id = byteZero;
    expectedInfo.revision_id     = byteZero;
    
    pac193x_getSensorInfo ( & actualInfo );
    
    TEST_ASSERT_EQUAL_UINT8( expectedInfo.product_id, actualInfo.product_id );
    TEST_ASSERT_EQUAL_UINT8( expectedInfo.manufacturer_id, actualInfo.manufacturer_id );
    TEST_ASSERT_EQUAL_UINT8( expectedInfo.revision_id, actualInfo.revision_id );
  }

void pac193x_Memory_not_passed_to_GetSensorInfo_remains_untouched ( void )
  {
    uint8_t      memory[512];
    pac193x_info info;
    
    memset ( memory, 0, 512 );
    
    pac193x_getSensorInfo ( & info );
    
    TEST_ASSERT_EACH_EQUAL_UINT8( 0, memory, 512 );
  }

/* endregion */
/* region pac193x_GetMeasurementForChannel */

void pac193x_GetMeasurementForChannel_return_SEND_COMMAND_ERROR_if_hardware_fails ( void )
  {
    float result;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_SEND_COMMAND_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_return_SEND_COMMAND_ERROR_if_ACK_missing ( void )
  {
    float result;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_SEND_COMMAND_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_return_RECEIVE_DATA_ERROR_if_hardware_fails ( void )
  {
    float result;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_RECEIVE_DATA_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_return_RECEIVE_DATA_ERROR_if_ACK_missing ( void )
  {
    float result;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_RECEIVE_DATA_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_return_INVALID_CHANNEL_error_if_channel_wrong ( void )
  {
    float result;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( 0x10, PAC193X_VSOURCE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_INVALID_CHANNEL, errorCode );
  }


void pac193x_GetMeasurementForChannel_read__successful_value_VSOURCE ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_VSOURCE ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t expected_rawValue = ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    expectedValue = ( 32.0f * ( ( ( float ) expected_rawValue ) / UNIPOLAR_VOLTAGE_DENOMINATOR ) );
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_VSOURCE_AVR ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE_AVG, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_VSOURCE_AVR ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t expected_rawValue = ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    expectedValue = ( 32.0f * ( ( ( float ) expected_rawValue ) / UNIPOLAR_VOLTAGE_DENOMINATOR ) );
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSOURCE_AVG, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_VSENSE ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSENSE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_VSENSE ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t expected_rawValue = ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    expectedValue = 0.1f * ( ( float ) expected_rawValue ) / UNIPOLAR_VOLTAGE_DENOMINATOR;
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSENSE, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_VSENSE_AVG ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSENSE_AVG, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_VSENSE_AVG ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t expected_rawValue = ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    expectedValue = 0.1f * ( ( float ) expected_rawValue ) / UNIPOLAR_VOLTAGE_DENOMINATOR;
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_VSENSE_AVG, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_ISENSE ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_ISENSE, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_ISENSE ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t expected_rawValue = ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    float    FSC               = 0.1f / R_SENSE[ usedChannelIndex ];
    expectedValue = FSC * ( ( ( float ) expected_rawValue ) / UNIPOLAR_VOLTAGE_DENOMINATOR );
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_ISENSE, & actualValue );
    
    TEST_ASSERT_EQUAL( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_ISENSE_AVG ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_ISENSE_AVG, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_ISENSE_AVG ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t expected_rawValue = ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    float    FSC               = 0.1f / R_SENSE[ usedChannelIndex ];
    expectedValue = FSC * ( ( ( float ) expected_rawValue ) / UNIPOLAR_VOLTAGE_DENOMINATOR );
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_ISENSE_AVG, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_PACTUAL ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_PACTUAL, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_PACTUAL ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t rawValue         = ( ( ( uint64_t ) byteZero << 24 ) | ( ( uint64_t ) byteZero << 16 ) | ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero );
    float    expectedRawValue = ( float ) rawValue;
    float    powerFSR         = 3.2f / R_SENSE[ usedChannelIndex ];
    float    pProp            = expectedRawValue / UNIPOLAR_POWER_DENOMINATOR;
    expectedValue = powerFSR * pProp;
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_PACTUAL, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void pac193x_GetMeasurementForChannel_read__successful_value_ENERGY ( void )
  {
    float result;
    
    pac193x_errorCode errorCode = pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_ENERGY, & result );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

void pac193x_GetMeasurementForChannel_read__correct_value_ENERGY ( void )
  {
    float expectedValue = 0, actualValue = 0;
    
    uint64_t rawValue         = ( ( uint64_t ) byteZero << 40 ) | ( ( uint64_t ) byteZero << 32 ) | ( ( uint64_t ) byteZero << 24 ) | ( ( uint64_t ) byteZero
            << 16 ) | ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    float    expectedRawValue = ( float ) rawValue;
    float    powerFSR         = 3.2f / R_SENSE[ usedChannelIndex ];
    expectedValue = expectedRawValue * powerFSR / ( ENERGY_DENOMINATOR * SAMPLING_RATE );
    
    pac193x_getMeasurementForChannel ( PAC193X_CHANNEL02, PAC193X_ENERGY, & actualValue );
    
    TEST_ASSERT_EQUAL_FLOAT( expectedValue, actualValue );
  }

void testAssertUint64tEquals ( void )
  {
    uint64_t expected = 0x0000BEBEBEBEBEBE;
    uint64_t actual   = ( ( uint64_t ) byteZero << 40 ) | ( ( uint64_t ) byteZero << 32 ) | ( ( uint64_t ) byteZero << 24 ) | ( ( uint64_t ) byteZero
            << 16 ) | ( ( uint64_t ) byteZero << 8 ) | ( uint64_t ) byteZero;
    
    TEST_ASSERT_EQUAL_UINT64( expected, actual );
  }

/* endregion */
/* region pac193x_GetAllMeasurementsForChannel */

void pac193x_GetAllMeasurementsForChannel_return_SEND_COMMAND_ERROR_if_hardware_fails ( void )
  {
    pac193x_measurements measurements;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getAllMeasurementsForChannel ( PAC193X_CHANNEL02, & measurements );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_SEND_COMMAND_ERROR, errorCode );
  }

void pac193x_GetAllMeasurementsForChannel_return_SEND_COMMAND_ERROR_if_ACK_missing ( void )
  {
    pac193x_measurements measurements;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getAllMeasurementsForChannel ( PAC193X_CHANNEL02, & measurements );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_SEND_COMMAND_ERROR, errorCode );
  }

void pac193x_GetAllMeasurementsForChannel_return_RECEIVE_DATA_ERROR_if_hardware_fails ( void )
  {
    pac193x_measurements measurements;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getAllMeasurementsForChannel ( PAC193X_CHANNEL02, & measurements );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_RECEIVE_DATA_ERROR, errorCode );
  }

void pac193x_GetAllMeasurementsForChannel_return_RECEIVE_DATA_ERROR_if_ACK_missing ( void )
  {
    pac193x_measurements measurements;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getAllMeasurementsForChannel ( PAC193X_CHANNEL02, & measurements );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_RECEIVE_DATA_ERROR, errorCode );
  }

void pac193x_GetAllMeasurementsForChannel_read_successful ( void )
  {
    pac193x_measurements measurements;
    
    pac193x_errorCode errorCode = pac193x_getAllMeasurementsForChannel ( PAC193X_CHANNEL02, & measurements );
    
    TEST_ASSERT_EQUAL_UINT8( PAC193X_NO_ERROR, errorCode );
  }

/* endregion */


int main ( void )
  {
    UNITY_BEGIN( );
    
    RUN_TEST( pac193x_Memory_not_passed_to_GetSensorInfo_remains_untouched );
    RUN_TEST( pac193x_GetSensorInfo_return_SEND_COMMAND_ERROR_if_hardware_fails );
    RUN_TEST( pac193x_GetSensorInfo_return_SEND_COMMAND_ERROR_if_ACK_missing );
    RUN_TEST( pac193x_GetSensorInfo_return_RECEIVE_DATA_ERROR_if_hardware_fails );
    RUN_TEST( pac193x_GetSensorInfo_return_RECEIVE_DATA_ERROR_if_ACK_missing );
    RUN_TEST( pac193x_GetSensorInfo_read_successful );
    RUN_TEST( pac193x_GetSensorInfo_read_correct_value );
    
    RUN_TEST( pac193x_GetMeasurementForChannel_return_SEND_COMMAND_ERROR_if_hardware_fails );
    RUN_TEST( pac193x_GetMeasurementForChannel_return_SEND_COMMAND_ERROR_if_ACK_missing );
    RUN_TEST( pac193x_GetMeasurementForChannel_return_RECEIVE_DATA_ERROR_if_hardware_fails );
    RUN_TEST( pac193x_GetMeasurementForChannel_return_RECEIVE_DATA_ERROR_if_ACK_missing );
    RUN_TEST( pac193x_GetMeasurementForChannel_return_INVALID_CHANNEL_error_if_channel_wrong );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_VSOURCE );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_VSOURCE );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_VSOURCE_AVR );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_VSOURCE_AVR );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_VSENSE );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_VSENSE );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_VSENSE_AVG );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_VSENSE_AVG );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_ISENSE );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_ISENSE );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_ISENSE_AVG );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_ISENSE_AVG );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_PACTUAL );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_PACTUAL );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__successful_value_ENERGY );
    RUN_TEST( pac193x_GetMeasurementForChannel_read__correct_value_ENERGY );
    
    RUN_TEST( testAssertUint64tEquals );
    
    RUN_TEST( pac193x_GetAllMeasurementsForChannel_return_SEND_COMMAND_ERROR_if_hardware_fails );
    RUN_TEST( pac193x_GetAllMeasurementsForChannel_return_SEND_COMMAND_ERROR_if_ACK_missing );
    RUN_TEST( pac193x_GetAllMeasurementsForChannel_return_RECEIVE_DATA_ERROR_if_hardware_fails );
    RUN_TEST( pac193x_GetAllMeasurementsForChannel_return_RECEIVE_DATA_ERROR_if_ACK_missing );
    RUN_TEST( pac193x_GetAllMeasurementsForChannel_read_successful );
    UnityPrint ( "Not testing for correct values. Values generated by pac193x_GetMeasurementForChannel(...) and have been tested before." );
    
    return UNITY_END( );
  }
