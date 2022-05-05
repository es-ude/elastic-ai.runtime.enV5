//
// Created by David P. Federl
//

#include "gpio.h"
#include "i2c.h"
#include "pac193x.h"
#include "unity.h"
#include <stdint.h>
#include <string.h>

/* region Variables */

static const float VOLTAGE_DENOMINATOR = (float)(1U << 16);
static const float POWER_DENOMINATOR   = (float)(1ULL << 32);
static const float ENERGY_DENOMINATOR  = (float)(1ULL << 28);
static const float SAMPLING_RATE       = 1024;
static float       R_SENSE[4] = {0.82f, 0.82f, 0, 0}; /* External resistor: 100kOhm and 3.3V -> 33A */
static uint8_t     HIGHEST_CHANNEL_NUMBER_IN_USE = 2;

static const uint8_t BYTE_ZERO                   = 0xBE;

/* endregion */
/* region Dummy Functions for GPIO */

GPIO_ErrorCode GPIO_setPower(GPIO_PIN pin, GPIO_POWER_MODE powerMode) { /* Just here to satisfy compiler */
  }

/* endregion */
/* region Dummy Functions for I2C */

void I2C_Init(i2c_inst_t *i2cHost, uint32_t baudrate, uint8_t sdaGPIO, uint8_t sclGPIO) {
    /* Just here to satisfy compiler */
  }

static I2C_ErrorCode (*I2C_WriteCommand_ptr)(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                                             uint8_t slaveAddress, i2c_inst_t *i2cHost);

I2C_ErrorCode I2C_WriteCommand(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                               uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_WriteCommand_ptr(commandBuffer, sizeOfCommandBuffer, slaveAddress, i2cHost);
  }

static I2C_ErrorCode I2C_WriteCommand_Hardware_defect(const uint8_t *commandBuffer,
                                                      uint16_t sizeOfCommandBuffer, uint8_t slaveAddress,
                                                      i2c_inst_t *i2cHost) {
    return I2C_INIT_ERROR;
  }

static I2C_ErrorCode I2C_WriteCommand_ACK_missing(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                                                  uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_ACK_ERROR;
  }

static I2C_ErrorCode I2C_WriteCommand_Pass(const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer,
                                           uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_NO_ERROR;
  }

static I2C_ErrorCode (*I2C_ReadCommand_ptr)(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                            uint8_t slaveAddress, i2c_inst_t *i2cHos);

I2C_ErrorCode I2C_ReadData(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer, uint8_t slaveAddress,
                           i2c_inst_t *i2cHost) {
    return I2C_ReadCommand_ptr(responseBuffer, sizeOfResponseBuffer, slaveAddress, i2cHost);
  }

static I2C_ErrorCode I2C_ReadCommand_Hardware_defect(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                                     uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_INIT_ERROR;
  }

static I2C_ErrorCode I2C_ReadCommand_ACK_missing(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                                 uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    return I2C_ACK_ERROR;
  }

static I2C_ErrorCode I2C_ReadCommand_Pass(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer,
                                          uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    /* generate sample data without any real world connection to test implementation */
    for (uint8_t i = 0; i < sizeOfResponseBuffer; i++) {
        responseBuffer[i] = BYTE_ZERO;
      }
    
    return 0x00;
  }

/* endregion */

void setUp(void) {
    /* Default: Point to Pass */
    I2C_WriteCommand_ptr = I2C_WriteCommand_Pass;
    I2C_ReadCommand_ptr  = I2C_ReadCommand_Pass;
    
    pac193x_setResistanceAtSense(R_SENSE);
    pac193x_setNumberOfChannelsInUse(HIGHEST_CHANNEL_NUMBER_IN_USE);
  }

void tearDown(void) {}

/* region pac193x_GetSensorInfo */

void pac193x_GetSensorInfo_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    pac193x_info info;
    I2C_WriteCommand_ptr        = I2C_WriteCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo(&info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
  }
void pac193x_GetSensorInfo_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    pac193x_info info;
    I2C_WriteCommand_ptr        = I2C_WriteCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo(&info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
  }
void pac193x_GetSensorInfo_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    pac193x_info info;
    I2C_ReadCommand_ptr         = I2C_ReadCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo(&info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
  }
void pac193x_GetSensorInfo_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    pac193x_info info;
    I2C_ReadCommand_ptr         = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo(&info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
  }

void pac193x_GetSensorInfo_read_successful(void) {
    pac193x_info info;
    
    pac193x_errorCode errorCode = pac193x_getSensorInfo(&info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetSensorInfo_read_correct_value(void) {
    pac193x_info expectedInfo, actualInfo;
    
    expectedInfo.product_id      = BYTE_ZERO;
    expectedInfo.manufacturer_id = BYTE_ZERO;
    expectedInfo.revision_id     = BYTE_ZERO;
    
    pac193x_getSensorInfo(&actualInfo);
    
    TEST_ASSERT_EQUAL_UINT8(expectedInfo.product_id, actualInfo.product_id);
    TEST_ASSERT_EQUAL_UINT8(expectedInfo.manufacturer_id, actualInfo.manufacturer_id);
    TEST_ASSERT_EQUAL_UINT8(expectedInfo.revision_id, actualInfo.revision_id);
  }

void pac193x_Memory_not_passed_to_GetSensorInfo_remains_untouched(void) {
    uint8_t      memory[512];
    pac193x_info info;
    
    memset(memory, 0, 512);
    
    pac193x_getSensorInfo(&info);
    
    TEST_ASSERT_EACH_EQUAL_UINT8(0, memory, 512);
  }

/* endregion */
/* region pac193x_GetMeasurement */

void pac193x_GetMeasurement_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    I2C_WriteCommand_ptr               = I2C_WriteCommand_Hardware_defect;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
  }
void pac193x_GetMeasurement_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    I2C_WriteCommand_ptr               = I2C_WriteCommand_ACK_missing;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
  }
void pac193x_GetMeasurement_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    I2C_ReadCommand_ptr                = I2C_ReadCommand_Hardware_defect;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
  }
void pac193x_GetMeasurement_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    I2C_ReadCommand_ptr                = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
  }

void pac193x_GetMeasurement_read_successful_value_VSOURCE(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_VSOURCE(void) {
    float                  expectedValue, actualValue;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    uint64_t expected_rawValue         = ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    expectedValue                      = (32.0f * (((float)expected_rawValue) / VOLTAGE_DENOMINATOR));
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_VSOURCE_AVR(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE_AVG;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_VSOURCE_AVR(void) {
    float                  expectedValue, actualValue;
    pac193x_valueToMeasure measurement = PAC193X_VSOURCE_AVG;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    uint64_t expected_rawValue         = ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    expectedValue                      = (32.0f * (((float)expected_rawValue) / VOLTAGE_DENOMINATOR));
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_VSENSE(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSENSE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_VSENSE(void) {
    float                  expectedValue = 0;
    float                  actualValue   = 0;
    pac193x_valueToMeasure measurement   = PAC193X_VSENSE;
    pac193x_channel        channel       = PAC193X_CHANNEL_SENSORS;
    
    uint64_t expected_rawValue           = ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    expectedValue                        = 0.1f * ((float)expected_rawValue) / VOLTAGE_DENOMINATOR;
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_VSENSE_AVG(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_VSENSE_AVG;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_VSENSE_AVG(void) {
    float                  expectedValue, actualValue;
    pac193x_valueToMeasure measurement = PAC193X_VSENSE_AVG;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    uint64_t expected_rawValue         = ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    expectedValue                      = 0.1f * ((float)expected_rawValue) / VOLTAGE_DENOMINATOR;
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_ISENSE(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_ISENSE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_ISENSE(void) {
    float                  expectedValue, actualValue;
    pac193x_valueToMeasure measurement = PAC193X_ISENSE;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    uint64_t expected_rawValue         = ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    float    FSC                       = 0.1f / R_SENSE[channel - 1];
    expectedValue                      = FSC * (((float)expected_rawValue) / VOLTAGE_DENOMINATOR);
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    TEST_ASSERT_EQUAL(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_ISENSE_AVG(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_ISENSE_AVG;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_ISENSE_AVG(void) {
    float                  expectedValue, actualValue;
    pac193x_valueToMeasure measurement = PAC193X_ISENSE_AVG;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    uint64_t expected_rawValue         = ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    float    FSC                       = 0.1f / R_SENSE[channel - 1];
    expectedValue                      = FSC * (((float)expected_rawValue) / VOLTAGE_DENOMINATOR);
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_PACTUAL(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_PACTUAL;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_PACTUAL(void) {
    float                  expectedValue, actualValue;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    pac193x_valueToMeasure measurement = PAC193X_PACTUAL;
    
    uint64_t rawValue                  = (((uint64_t)BYTE_ZERO << 24) | ((uint64_t)BYTE_ZERO << 16) |
                                          ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO);
    float    expectedRawValue          = (float)rawValue;
    float    powerFSR                  = 3.2f / R_SENSE[channel - 1];
    float    pProp                     = expectedRawValue / POWER_DENOMINATOR;
    expectedValue                      = powerFSR * pProp;
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void pac193x_GetMeasurement_read_successful_value_ENERGY(void) {
    float                  result;
    pac193x_valueToMeasure measurement = PAC193X_ENERGY;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode        = pac193x_getMeasurementForChannel(channel, measurement, &result);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }
void pac193x_GetMeasurement_read_correct_value_ENERGY(void) {
    float                  expectedValue, actualValue;
    pac193x_valueToMeasure measurement = PAC193X_ENERGY;
    pac193x_channel        channel     = PAC193X_CHANNEL_SENSORS;
    
    uint64_t rawValue                  = ((uint64_t)BYTE_ZERO << 40) | ((uint64_t)BYTE_ZERO << 32) |
                                         ((uint64_t)BYTE_ZERO << 24) | ((uint64_t)BYTE_ZERO << 16) |
                                         ((uint64_t)BYTE_ZERO << 8) | (uint64_t)BYTE_ZERO;
    float expectedRawValue = (float)rawValue;
    float powerFSR         = 3.2f / R_SENSE[channel - 1];
    expectedValue          = expectedRawValue * powerFSR / (ENERGY_DENOMINATOR * SAMPLING_RATE);
    
    pac193x_getMeasurementForChannel(channel, measurement, &actualValue);
    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
  }

void testAssertUint64tEquals(void) {
    uint64_t expected = 0x0000BEBEBEBEBEBE;
    uint64_t actual   = ((uint64_t)BYTE_ZERO << 40) | ((uint64_t)BYTE_ZERO << 32) |
                        ((uint64_t)BYTE_ZERO << 24) | ((uint64_t)BYTE_ZERO << 16) | ((uint64_t)BYTE_ZERO << 8) |
                        (uint64_t)BYTE_ZERO;
    
    TEST_ASSERT_EQUAL_UINT64(expected, actual);
  }

/* endregion */
/* region pac193x_GetAllMeasurementsForChannel */

void pac193x_GetAllMeasurementsForChannel_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    pac193x_channel      channel = PAC193X_CHANNEL_SENSORS;
    pac193x_measurements measurements;
    I2C_WriteCommand_ptr        = I2C_WriteCommand_Hardware_defect;
    
    pac193x_errorCode errorCode = pac193x_getAllMeasurementsForChannel(channel, &measurements);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
  }
void pac193x_GetAllMeasurementsForChannel_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    pac193x_measurements measurements;
    pac193x_channel      channel = PAC193X_CHANNEL_SENSORS;
    I2C_WriteCommand_ptr         = I2C_WriteCommand_ACK_missing;
    
    pac193x_errorCode errorCode  = pac193x_getAllMeasurementsForChannel(channel, &measurements);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
  }
void pac193x_GetAllMeasurementsForChannel_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    pac193x_measurements measurements;
    pac193x_channel      channel = PAC193X_CHANNEL_SENSORS;
    I2C_ReadCommand_ptr          = I2C_ReadCommand_Hardware_defect;
    
    pac193x_errorCode errorCode  = pac193x_getAllMeasurementsForChannel(channel, &measurements);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
  }
void pac193x_GetAllMeasurementsForChannel_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    pac193x_measurements measurements;
    pac193x_channel      channel = PAC193X_CHANNEL_SENSORS;
    I2C_ReadCommand_ptr          = I2C_ReadCommand_ACK_missing;
    
    pac193x_errorCode errorCode  = pac193x_getAllMeasurementsForChannel(channel, &measurements);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
  }

void pac193x_GetAllMeasurementsForChannel_read_successful(void) {
    pac193x_measurements measurements;
    pac193x_channel      channel = PAC193X_CHANNEL_SENSORS;
    
    pac193x_errorCode errorCode  = pac193x_getAllMeasurementsForChannel(channel, &measurements);
    
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
  }

/* Not testing for actual correct values -> Values are generated by method pac193x_GetMeasurements(...) */

/* endregion */
/* region pac193x_GetEnergyAndResetAccumulator */

/* Not testing method -> values generated by method pac193x_GetMeasurement */

/* endregion */

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(pac193x_Memory_not_passed_to_GetSensorInfo_remains_untouched);
    RUN_TEST(pac193x_GetSensorInfo_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(pac193x_GetSensorInfo_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(pac193x_GetSensorInfo_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(pac193x_GetSensorInfo_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(pac193x_GetSensorInfo_read_successful);
    RUN_TEST(pac193x_GetSensorInfo_read_correct_value);
    
    RUN_TEST(pac193x_GetMeasurement_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(pac193x_GetMeasurement_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(pac193x_GetMeasurement_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(pac193x_GetMeasurement_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_VSOURCE);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_VSOURCE);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_VSOURCE_AVR);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_VSOURCE_AVR);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_VSENSE);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_VSENSE);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_VSENSE_AVG);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_VSENSE_AVG);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_ISENSE);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_ISENSE);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_ISENSE_AVG);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_ISENSE_AVG);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_PACTUAL);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_PACTUAL);
    RUN_TEST(pac193x_GetMeasurement_read_successful_value_ENERGY);
    RUN_TEST(pac193x_GetMeasurement_read_correct_value_ENERGY);
    RUN_TEST(testAssertUint64tEquals);
    
    RUN_TEST(pac193x_GetAllMeasurementsForChannel_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(pac193x_GetAllMeasurementsForChannel_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(pac193x_GetAllMeasurementsForChannel_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(pac193x_GetAllMeasurementsForChannel_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(pac193x_GetAllMeasurementsForChannel_read_successful);
    
    return UNITY_END();
  }
