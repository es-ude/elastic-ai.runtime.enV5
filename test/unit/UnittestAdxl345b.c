#include "Adxl345b.h"
#include "I2cUnitTest.h"
#include <unity.h>

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost
 */

adxl345bSensorConfiguration_t sensor;

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForAdxl345b;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345b;

    adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
}

void tearDown(void) {}

/* region adxl345bReadSerialNumber */

void adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwarFails(void) {
    uint8_t serialNumber;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing(void) {
    uint8_t serialNumber;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwarFails(void) {
    uint8_t serialNumber;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing(void) {
    uint8_t serialNumber;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberReadSuccessful(void) {
    uint8_t serialNumber;

    uint8_t err = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, err);
}

void adxl345bReadSerialNumberReadCorrectValue(void) {
    uint8_t expected_serialNumber, actual_serialNumber;

    /* fill expected with random generated */
    expected_serialNumber = byteZero;

    adxl345bReadSerialNumber(sensor, &actual_serialNumber);
    TEST_ASSERT_EQUAL_UINT8(expected_serialNumber, actual_serialNumber);
}

/* endregion */

/* region adxl345bReadMeasurementOneShot */

void adxl345bReadMeasurementOneShotGetSendCommandFail_errorIfHardwarFails(void) {
    uint8_t rawDataSizeSix[6];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadMeasurementOneShot(sensor, rawDataSizeSix);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadMeasurementOneShotGetSendCommandFail_errorIfAckMissing(void) {
    uint8_t rawDataSizeSix[6];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadMeasurementOneShot(sensor, rawDataSizeSix);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadMeasurementOneShotGetReceiveDataFail_errorIfHardwarFails(void) {
    uint8_t rawDataSizeSix[6];
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadMeasurementOneShot(sensor, rawDataSizeSix);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadMeasurementOneShotGetReceiveDataFail_errorIfAckMissing(void) {
    uint8_t rawDataSizeSix[6];
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadMeasurementOneShot(sensor, rawDataSizeSix);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadMeasurementOneShotReadSuccessful(void) {
    uint8_t rawDataSizeSix[6];

    uint8_t err = adxl345bReadMeasurementOneShot(sensor, rawDataSizeSix);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, err);
}

void adxl345bReadMeasurementOneShotReadCorrectValue(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawDataSizeSix[sizeOfRawData];

    adxl345bReadMeasurementOneShot(sensor, rawDataSizeSix);
    for(int i = 0; i < sizeOfRawData; i ++){
        TEST_ASSERT_EQUAL_UINT8(byteZero, rawDataSizeSix[i]);
    }

}

void adxl345bConvertDataXYZCorrectValue(void) {
    /* test assumes that 2G Full Range is the used Range */

    float expected_xAxis = 0, expected_yAxis = 0, expected_zAxis = 0;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    const uint8_t MSB_MASK = 0b00000011;
    const float SCALE_FACTOR_FOR_RANGE = 0.0043f;
    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
    uint8_t topByte = byteZero & MSB_MASK;

    /* set rawData */
    uint8_t sizeOfRawData = 6;
    uint8_t rawDataSizeSix[sizeOfRawData];
    for(int i = 0; i < sizeOfRawData; i ++){
            rawDataSizeSix[i] = byteZero;
    }

    /* fill expected with random generated */
    if (topByte <= (MSB_MASK >> 1)) {
        /* CASE: positive value */
        int rawValue = (int)(((uint16_t)(topByte & MSB_MASK) << 8) | (uint16_t)byteZero);
        float realValue = (float)rawValue * SCALE_FACTOR_FOR_RANGE;
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
        uint16_t rawValue = ((uint16_t)(topByte & (MSB_MASK >> 1)) << 8) | (uint16_t)byteZero;
        rawValue = (rawValue - 0x0001) ^ (((MSB_MASK >> 1) << 8) | 0x00FF);
        float realValue = (-1) * (float)rawValue * SCALE_FACTOR_FOR_RANGE;
        expected_xAxis = expected_yAxis = expected_zAxis = realValue;
    }

    adxl345bConvertDataXYZ(&actual_xAxis, &actual_yAxis, &actual_zAxis, rawDataSizeSix);
    TEST_ASSERT_EQUAL_FLOAT(expected_xAxis, actual_xAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_yAxis, actual_yAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_zAxis, actual_zAxis);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwarFails);
    RUN_TEST(adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwarFails);
    RUN_TEST(adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadSerialNumberReadSuccessful);
    RUN_TEST(adxl345bReadSerialNumberReadCorrectValue);

    RUN_TEST(adxl345bReadMeasurementOneShotGetSendCommandFail_errorIfHardwarFails);
    RUN_TEST(adxl345bReadMeasurementOneShotGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadMeasurementOneShotGetReceiveDataFail_errorIfHardwarFails);
    RUN_TEST(adxl345bReadMeasurementOneShotGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadMeasurementOneShotReadSuccessful);
    RUN_TEST(adxl345bReadMeasurementOneShotReadCorrectValue);

    RUN_TEST(adxl345bConvertDataXYZCorrectValue);

    return UNITY_END();
}
