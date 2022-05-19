#define TEST_BUILD

#include "i2c/i2c_test.h"
#include "adxl345b_public.h"
#include "unity.h"

void setUp(void) {
    /* Default: Point to Pass */
    I2C_WriteCommand_ptr = I2C_WriteCommand_Pass_for_ADXL345B;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Pass_for_ADXL345B;

    adxl345b_changeMeasurementRange(ADXL345B_2G_RANGE);
}

void tearDown(void) {
}

/* region ADXL345B_ReadSerialNumber */

void ADXL345B_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    uint8_t serialNumber;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    adxl345b_errorCode errorCode = adxl345b_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void ADXL345B_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    uint8_t serialNumber;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    adxl345b_errorCode errorCode = adxl345b_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void ADXL345B_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    uint8_t serialNumber;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    adxl345b_errorCode errorCode = adxl345b_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void ADXL345B_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    uint8_t serialNumber;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    adxl345b_errorCode errorCode = adxl345b_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void ADXL345B_ReadSerialNumber_read_successful(void) {
    uint8_t serialNumber;

    uint8_t err = adxl345b_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, err);
}

void ADXL345B_ReadSerialNumber_read_correct_value(void) {
    uint8_t expected_serialNumber, actual_serialNumber;

    /* fill expected with random generated */
    expected_serialNumber = byteZero;

    adxl345b_readSerialNumber(&actual_serialNumber);
    TEST_ASSERT_EQUAL_UINT8(expected_serialNumber, actual_serialNumber);
}

/* endregion */
/* region ADXL345B_ReadMeasurements */

void ADXL345B_ReadMeasurements_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    float xAxis = 0, yAxis = 0, zAxis = 0;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    adxl345b_errorCode errorCode = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void ADXL345B_ReadMeasurements_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    float xAxis = 0, yAxis = 0, zAxis = 0;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    adxl345b_errorCode errorCode = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void ADXL345B_ReadMeasurements_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    float xAxis = 0, yAxis = 0, zAxis = 0;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    adxl345b_errorCode errorCode = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void ADXL345B_ReadMeasurements_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    float xAxis = 0, yAxis = 0, zAxis = 0;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    adxl345b_errorCode errorCode = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void ADXL345B_ReadMeasurements_read_successful(void) {
    float xAxis = 0, yAxis = 0, zAxis = 0;

    uint8_t err = adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, err);
}

void ADXL345B_ReadMeasurements_read_correct_value(void) {
    /* test assumes that 2G Full Range is the used Range */

    float expected_xAxis = 0, expected_yAxis = 0, expected_zAxis = 0;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    const uint8_t MSB_MASK = 0b00000011;
    const float SCALE_FACTOR_FOR_RANGE = 0.0043f;
    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
    uint8_t topByte = byteZero & MSB_MASK;

    /* fill expected with random generated */
    if (topByte <= (MSB_MASK >> 1)) {
        /* CASE: positive value */
        int rawValue = (int) (((uint16_t) (topByte & MSB_MASK) << 8) | (uint16_t) byteZero);
        float realValue = (float) rawValue * SCALE_FACTOR_FOR_RANGE;
        expected_xAxis = expected_yAxis = expected_zAxis = realValue;
    } else {
        /* CASE: negative value
         *
         * 1. revert 10 bit two complement
         * -> number-1 and Flip least 9 bits
         * 2. convert to float value
         * 3. multiply with (-1)
         * 4. multiply with scale factor
         */
        uint16_t rawValue = ((uint16_t) (topByte & (MSB_MASK >> 1)) << 8) | (uint16_t) byteZero;
        rawValue = (rawValue - 0x0001) ^ (((MSB_MASK >> 1) << 8) | 0x00FF);
        float realValue = (-1) * (float) rawValue * SCALE_FACTOR_FOR_RANGE;
        expected_xAxis = expected_yAxis = expected_zAxis = realValue;
    }

    adxl345b_readMeasurements(&actual_xAxis, &actual_yAxis, &actual_zAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_xAxis, actual_xAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_yAxis, actual_yAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_zAxis, actual_zAxis);
}

/* endregion */
// region adxl345b_writeConfigurationToSensor

// endregion
// region adxl345b_changeMeasurementRange

// endregion

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(ADXL345B_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(ADXL345B_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(ADXL345B_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(ADXL345B_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(ADXL345B_ReadSerialNumber_read_successful);
    RUN_TEST(ADXL345B_ReadSerialNumber_read_correct_value);

    RUN_TEST(ADXL345B_ReadMeasurements_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(ADXL345B_ReadMeasurements_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(ADXL345B_ReadMeasurements_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(ADXL345B_ReadMeasurements_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(ADXL345B_ReadMeasurements_read_successful);
    RUN_TEST(ADXL345B_ReadMeasurements_read_correct_value);

    return UNITY_END();
}
