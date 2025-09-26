#include "I2cUnitTest.h"
#include "eai/sensor/Adxl345b.h"
#include "eai/sensor/Adxl345bTypedefs.h"
#include <memory.h>
#include <unity.h>

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost
 */

adxl345bSensorConfiguration_t sensor;
uint32_t numberOfSamples;
uint8_t serialNumber;
uint32_t microseconds;

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForAdxl345b;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInBypassMode;

    numberOfSamples = 10;
    microseconds = 1;
}

void tearDown(void) {}

/* region adxl345bReadSerialNumber */

void adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwarFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwarFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberReadSuccessful(void) {
    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bReadSerialNumberReadCorrectValue(void) {
    uint8_t expected_serialNumber, actual_serialNumber;

    /* fill expected with random generated */
    expected_serialNumber = byteZero;

    adxl345bReadSerialNumber(sensor, &actual_serialNumber);
    TEST_ASSERT_EQUAL_UINT8(expected_serialNumber, actual_serialNumber);
}

/* endregion adxl345bReadSerialNumber */

/* region adxl345bChangeMeasurementRange */
void adxl345bChangeMeasurementRangeGetSendCommandFail_errorIfHardwarFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bChangeMeasurementRangeGetSendCommandFail_errorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bChangeMeasurementRangeGetReceiveDataFail_errorIfHardwarFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bChangeMeasurementRangeGetReceiveDataFail_errorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bChangeMeasurementRangeWrongRange(void) {
    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, 8);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RANGE_ERROR, errorCode);
}

void adxl345bChangeMeasurementRangeChangeSuccessful(void) {
    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_NO_ERROR, errorCode);
}
/* endregion adxl345bChangeMeasurementRange */

/* region adxl345bGetSingleMeasurement */

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfHardwareFails(void) {
    adxl345bRawData_t rawData;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing(void) {
    adxl345bRawData_t rawData;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails(void) {
    adxl345bRawData_t rawData;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing(void) {
    adxl345bRawData_t rawData;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadSuccessful(void) {
    adxl345bRawData_t rawData;
    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadCorrectValue(void) {
    adxl345bRawData_t rawData;
    uint8_t expectedRawData = byteZero;

    adxl345bGetSingleMeasurement(sensor, &rawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, rawData.data, sizeof(adxl345bRawData_t));
}
/* endregion adxl345bGetSingleMeasurement */

/* region adxl345bGetMultipleMeasurements */

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails(void) {
    adxl345bRawData_t samples[numberOfSamples];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing(void) {
    adxl345bRawData_t samples[numberOfSamples];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails(void) {
    adxl345bRawData_t samples[numberOfSamples];
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing(void) {
    adxl345bRawData_t samples[numberOfSamples];
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsReadSuccessful(void) {
    adxl345bRawData_t samples[numberOfSamples];

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_NO_ERROR, errorCode);
}
/* region GetMultipleMeasurementsReadCorrectValues */
void adxl345bGetMultipleMeasurementsReadCorrectValuesInBypassMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

/* endregion GetMultipleMeasurementsReadCorrectValues */
/* endregion adxl345bGetMultipleMeasurements */

/* region adxl345bGetMeasurementsForNMicroseconds */

void adxl345bGetMeasurementsForNMicrosecondsGetSendCommandFail_errorIfHardwareFails(void) {
    adxl345bRawData_t samples[numberOfSamples];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;
    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);

    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsGetSendCommandFail_errorIfAckMissing(void) {
    adxl345bRawData_t samples[numberOfSamples];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsGetReceiveDataFail_errorIfHardwareFails(void) {
    adxl345bRawData_t samples[numberOfSamples];

    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsGetReceiveDataFail_errorIfAckMissing(void) {
    adxl345bRawData_t samples[numberOfSamples];

    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInBypassMode(void) {
    adxl345bRawData_t samples[numberOfSamples];

    microseconds = 2;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_PARM_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInStreamMode(void) {
    microseconds = 30;
    adxl345bRawData_t samples[numberOfSamples];

    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);

    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_PARM_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInFifoMode(void) {
    adxl345bRawData_t samples[numberOfSamples];
    microseconds = 40;

    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_PARM_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInTriggerMode(void) {
    adxl345bRawData_t samples[numberOfSamples];

    microseconds = 50;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_PARM_ERROR, errorCode);
}

/* region GetMeasurementsForNmicrosecondsReadCorrectValues */
void adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInStreamMode(void) {
    microseconds = 60;

    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    if (errorCode != ADXL345B_PARM_ERROR) {
        TEST_FAIL();
    }
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples,
                                 numberOfSamples * sizeof(adxl345bRawData_t));
}

void adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInFifoMode(void) {
    microseconds = 800;

    /*generate Array for Data*/

    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteZero;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    if (errorCode != ADXL345B_PARM_ERROR) {
        TEST_FAIL();
    }
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples,
                                 numberOfSamples * sizeof(adxl345bRawData_t));
}

void adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInTriggerMode(void) {
    microseconds = 1000;

    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteZero;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    if (errorCode != ADXL345B_PARM_ERROR) {
        TEST_FAIL();
    }
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples,
                                 numberOfSamples * sizeof(adxl345bRawData_t));
}

/* endregion GetMeasurementsForNmicrosecondsReadCorrectValues */
/* endregion adxl345bGetMeasurementsForNMicroseconds */

void adxl345bConvertDataXYZCorrectValueWithFullResON(void) {
    float expected_xAxis = -1.213940f, expected_yAxis = -1.213940f, expected_zAxis = -1.213940f;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    /* set rawData */
    adxl345bRawData_t rawData;
    memset(&rawData, byteZero, sizeof(rawData));

    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bWithFullResON;
    adxl345bErrorCode_t errorCode =
        adxl345bConvertDataXYZ(sensor, &actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        printf("adxl345bConvertDataXYZ failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

    TEST_ASSERT_EQUAL_FLOAT(expected_xAxis, actual_xAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_yAxis, actual_yAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_zAxis, actual_zAxis);
}

void adxl345bConvertDataXYZCorrectValueWithFullResOFF(void) {
    /* test assumes 10bit-2G Range*/
    float expected_xAxis = -1.213940f, expected_yAxis = -1.213940f, expected_zAxis = -1.213940f;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    /* set rawData */
    adxl345bRawData_t rawData;
    memset(&rawData, byteZero, sizeof(rawData));

    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bWithFullResOFF;
    adxl345bErrorCode_t errorCode =
            adxl345bConvertDataXYZ(sensor, &actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        printf("adxl345bConvertDataXYZ failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

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

    RUN_TEST(adxl345bChangeMeasurementRangeGetSendCommandFail_errorIfHardwarFails);
    RUN_TEST(adxl345bChangeMeasurementRangeGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bChangeMeasurementRangeGetReceiveDataFail_errorIfHardwarFails);
    RUN_TEST(adxl345bChangeMeasurementRangeGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bChangeMeasurementRangeWrongRange);
    RUN_TEST(adxl345bChangeMeasurementRangeChangeSuccessful);

    RUN_TEST(adxl345bGetSingleMeasurementGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetSingleMeasurementReadSuccessful);
    RUN_TEST(adxl345bGetSingleMeasurementReadCorrectValue);

    RUN_TEST(adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadSuccessful);

    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInBypassMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerMode);

    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetReceiveDataFail_errorIfAckMissing);

    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInBypassMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInStreamMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInFifoMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInTriggerMode);

    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInBypassMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInStreamMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInFifoMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInTriggerMode);

    RUN_TEST(adxl345bConvertDataXYZCorrectValueWithFullResON);
    RUN_TEST(adxl345bConvertDataXYZCorrectValueWithFullResOFF);
    return UNITY_END();
}
