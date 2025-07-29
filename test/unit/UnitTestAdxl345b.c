#include <memory.h>
// #include <stdlib.h>
#include <unity.h>

#include "I2cUnitTest.h"
#include "eai/sensor/Adxl345b.h"
#include "eai/sensor/Adxl345bTypedefs.h"

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost
 */

adxl345bSensorConfiguration_t sensor;
uint32_t numberOfSamples;
uint8_t sizeOfSingleRawData;
uint8_t serialNumber;
uint32_t milliseconds;

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForAdxl345b;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInBypassMode;

    adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);

    numberOfSamples = 2;
    sizeOfSingleRawData = 6;
    milliseconds = 1;
}

void tearDown(void) {}

/* region adxl345bReadSerialNumber */

void adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwarFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwarFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberReadSuccessful(void) {
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

/* endregion adxl345bReadSerialNumber */

/* region adxl345bGetSingleMeasurement */

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfHardwareFails(void) {
    uint8_t rawData[sizeOfSingleRawData];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing(void) {
    uint8_t rawData[sizeOfSingleRawData];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails(void) {
    uint8_t rawData[sizeOfSingleRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing(void) {
    uint8_t rawData[sizeOfSingleRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadSuccessful(void) {
    uint8_t rawData[sizeOfSingleRawData];
    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadCorrectValue(void) {
    uint8_t rawData[sizeOfSingleRawData];
    memset(rawData, 0, sizeOfSingleRawData);

    adxl345bGetSingleMeasurement(sensor, rawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(byteZero, rawData, sizeOfSingleRawData);
}
/* endregion adxl345bGetSingleMeasurement */

/* region adxl345bGetMultipleMeasurements */

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsReadSuccessful(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInStreamMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    // check if some data is 0
    uint8_t counter = 0;
    for (int i = 0; i < (sizeOfRequestedRawData); i += sizeOfSingleRawData) {
        if (samples[i] == 0) {
            counter++;

        } else {
            printf("%d should be 10011111 but is: %b \n", i, samples[i]);
            counter = 0;
        }
    }
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInFifoMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    // check if some data is 0
    uint8_t counter = 0;
    for (int i = 0; i < (sizeOfRequestedRawData); i += sizeOfSingleRawData) {
        if (samples[i] == 0) {
            counter++;

        } else {
            printf("%d should be 10011111 but is: %b \n", i, samples[i]);
            counter = 0;
        }
    }
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInTriggerMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    // check if some data is 0
    uint8_t counter = 0;
    for (int i = 0; i < (sizeOfRequestedRawData); i += sizeOfSingleRawData) {
        if (samples[i] == 0) {
            counter++;
        } else {
            printf("%d should be 10011111 but is: %b \n", i, samples[i]);
            counter = 0;
        }
    }
    TEST_ASSERT_EQUAL(0, counter);
}

/* region GetMultipleMeasurementsReadCorrectValues */
void adxl345bGetMultipleMeasurementsReadCorrectValuesInBypassModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint8_t samples[sizeOfSingleRawData];
    memset(samples, 0, sizeOfSingleRawData);

    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfSingleRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfSingleRawData);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInBypassModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint8_t samples[sizeOfSingleRawData];
    uint8_t wrongSize = 1;
    memset(samples, 0, sizeOfSingleRawData + wrongSize);

    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfSingleRawData + wrongSize);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfSingleRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfSingleRawData], wrongSize);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t wrongSize = 5;
    uint8_t samples[sizeOfRequestedRawData + wrongSize];
    memset(samples, 0, sizeOfRequestedRawData + wrongSize);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData + wrongSize);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData], wrongSize);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t wrongSize = 1;
    uint8_t samples[sizeOfRequestedRawData + wrongSize];
    memset(samples, 0, sizeOfRequestedRawData + wrongSize);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData + wrongSize);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData], wrongSize);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t wrongSize = 3;
    uint8_t samples[sizeOfRequestedRawData + wrongSize];
    memset(samples, 0, sizeOfRequestedRawData + wrongSize);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

    adxl345bGetMultipleMeasurements(sensor, samples, sizeOfRequestedRawData + wrongSize);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData], wrongSize);
}
/* endregion GetMultipleMeasurementsReadCorrectValues */
/* endregion adxl345bGetMultipleMeasurements */

/* region adxl345bGetMeasurementsForNMilliseconds */

void adxl345bGetMeasurementsForNMillisecondsGetSendCommandFail_errorIfHardwareFails(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);

    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsGetSendCommandFail_errorIfAckMissing(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsGetReceiveDataFail_errorIfHardwareFails(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];

    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsGetReceiveDataFail_errorIfAckMissing(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];

    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInBypassMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);
    milliseconds = 2;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);

    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInStreamMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);
    milliseconds = 30;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);

    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInFifoMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    // ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }
    milliseconds = 40;

    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInTriggerMode(void) {
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);
    milliseconds = 50;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(
        sensor, samples, milliseconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

/* region GetMeasurementsForNMillisecondsReadCorrectValues */
void adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInStreamModeWithCorrectSizedArray(
    void) {
    milliseconds = 60;

    /*generate Array for Data*/
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds, &sizeOfRequestedRawData);

    if (sizeOfRequestedRawData < (numberOfSamples * sizeOfRequestedRawData)) {
        TEST_MESSAGE("case: (given milliseconds) < (reading-time until buffer is completely full)");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
        if (sizeOfRequestedRawData % 6 != 0) {
            TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData],
                                         (numberOfSamples * sizeOfSingleRawData) -
                                             sizeOfRequestedRawData);
            TEST_FAIL_MESSAGE("adxl345bGetMeasurementsForNMilliseconds probably has a problem with "
                              "handling the size of rawdata");
        }
    } else if (sizeOfRequestedRawData == (numberOfSamples * sizeOfRequestedRawData)) {
        TEST_MESSAGE("case: given buffer was too small for given milliseconds");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
    } else {
        TEST_FAIL_MESSAGE("adxl345bGetMeasurementsForNMilliseconds probably returns wrong "
                          "sizeOfRequestedRawData (or else)");
    }
}

void adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInStreamModeWithWrongSizedArray(void) {
    milliseconds = 70;

    /*generate Array for Data*/

    uint8_t wrongSize = 5;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData + wrongSize;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds, &sizeOfRequestedRawData);

    if (sizeOfRequestedRawData < (numberOfSamples * sizeOfRequestedRawData + wrongSize)) {
        TEST_MESSAGE("case: (given milliseconds) < (reading-time until buffer is completely full)");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
        TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData],
                                     (numberOfSamples * sizeOfSingleRawData + wrongSize) -
                                         sizeOfRequestedRawData);
    } else {
        TEST_MESSAGE("case: given buffer was too small for given milliseconds");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData - wrongSize);
        TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData - wrongSize], wrongSize);
    }
}

void adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInFifoModeWithCorrectSizedArray(void) {
    milliseconds = 800;

    /*generate Array for Data*/

    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);
    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds, &sizeOfRequestedRawData);

    if (sizeOfRequestedRawData < (numberOfSamples * sizeOfRequestedRawData)) {
        TEST_MESSAGE("case: (given milliseconds) < (reading-time until buffer is completely full)");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
        if (sizeOfRequestedRawData % 6 != 0) {
            TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData],
                                         (numberOfSamples * sizeOfSingleRawData) -
                                             sizeOfRequestedRawData);
            TEST_FAIL_MESSAGE("adxl345bGetMeasurementsForNMilliseconds probably has a problem with "
                              "handling the size of rawdata");
        }
    } else if (sizeOfRequestedRawData == (numberOfSamples * sizeOfRequestedRawData)) {
        TEST_MESSAGE("case: given buffer was too small for given milliseconds");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
    } else {
        TEST_FAIL_MESSAGE("adxl345bGetMeasurementsForNMilliseconds probably returns wrong "
                          "sizeOfRequestedRawData (or else)");
    }
}

void adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInFifoModeWithWrongSizedArray(void) {
    milliseconds = 900;

    /*generate Array for Data*/

    uint8_t wrongSize = 1;
    uint32_t sizeOfRequestedRawData = (numberOfSamples * sizeOfSingleRawData) + wrongSize;

    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);
    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds, &sizeOfRequestedRawData);

    if (sizeOfRequestedRawData < (numberOfSamples * sizeOfRequestedRawData + wrongSize)) {
        TEST_MESSAGE("case: (given milliseconds) < (reading-time until buffer is completely full)");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
        TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData],
                                     (numberOfSamples * sizeOfSingleRawData + wrongSize) -
                                         sizeOfRequestedRawData);
    } else {
        TEST_MESSAGE("case: given buffer was too small for given milliseconds");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData - wrongSize);
        TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData - wrongSize], wrongSize);
    }
}

void adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInTriggerModeWithCorrectSizedArray(
    void) {
    milliseconds = 1000;

    /*generate Array for Data*/

    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds, &sizeOfRequestedRawData);

    if (sizeOfRequestedRawData < (numberOfSamples * sizeOfRequestedRawData)) {
        TEST_MESSAGE("case: (given milliseconds) < (reading-time until buffer is completely full)");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
        if (sizeOfRequestedRawData % 6 != 0) {
            TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData],
                                         (numberOfSamples * sizeOfSingleRawData) -
                                             sizeOfRequestedRawData);
            TEST_FAIL_MESSAGE("adxl345bGetMeasurementsForNMilliseconds probably has a problem with "
                              "handling the size of rawdata");
        }
    } else if (sizeOfRequestedRawData == (numberOfSamples * sizeOfRequestedRawData)) {
        TEST_MESSAGE("case: given buffer was too small for given milliseconds");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
    } else {
        TEST_FAIL_MESSAGE("adxl345bGetMeasurementsForNMilliseconds probably returns wrong "
                          "sizeOfRequestedRawData (or else)");
    }
}

void adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInTriggerModeWithWrongSizedArray(
    void) {
    milliseconds = 11000;

    /*generate Array for Data*/
    uint8_t wrongSize = 2;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData + wrongSize;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds, &sizeOfRequestedRawData);

    if (sizeOfRequestedRawData < (numberOfSamples * sizeOfRequestedRawData + wrongSize)) {
        TEST_MESSAGE("case: (given milliseconds) < (reading-time until buffer is completely full)");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
        TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData],
                                     (numberOfSamples * sizeOfSingleRawData + wrongSize) -
                                         sizeOfRequestedRawData);
    } else {
        TEST_MESSAGE("case: given buffer was too small for given milliseconds");
        TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData - wrongSize);
        TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[sizeOfRequestedRawData - wrongSize], wrongSize);
    }
}
/* endregion GetMeasurementsForNMillisecondsReadCorrectValues */
/* endregion adxl345bGetMeasurementsForNMilliseconds */

void adxl345bConvertDataXYZCorrectValue(void) {
    /* test assumes that 2G Full Range is the used Range */

    float expected_xAxis = 0, expected_yAxis = 0, expected_zAxis = 0;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    const uint8_t MSB_MASK = 0b00000011;
    const float SCALE_FACTOR_FOR_RANGE = 0.0043f;
    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
    uint8_t topByte = byteZero & MSB_MASK;

    /* set rawData */
    uint8_t rawData[sizeOfSingleRawData];
    memset(rawData, 0, sizeOfSingleRawData);

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
    adxl345bGetSingleMeasurement(sensor, rawData);
    adxl345bConvertDataXYZ(&actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);

    TEST_ASSERT_EQUAL_FLOAT(expected_xAxis, actual_xAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_yAxis, actual_yAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_zAxis, actual_zAxis);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    // ReadSerialNumber
    RUN_TEST(adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwarFails);
    RUN_TEST(adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwarFails);
    RUN_TEST(adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadSerialNumberReadSuccessful);
    RUN_TEST(adxl345bReadSerialNumberReadCorrectValue);

    // GetSingleMeasurement
    RUN_TEST(adxl345bGetSingleMeasurementGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetSingleMeasurementReadSuccessful);
    RUN_TEST(adxl345bGetSingleMeasurementReadCorrectValue);

    // GetMultipleMeasurements
    RUN_TEST(adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadSuccessful);

    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithWrongSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithWrongSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithWrongSizedArray);

    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInStreamMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInFifoMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInTriggerMode);

    // GetMeasurementsForNMilliseconds
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsGetReceiveDataFail_errorIfAckMissing);

    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInBypassMode);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInStreamMode);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInFifoMode);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsReadSuccessfulInTriggerMode);

    RUN_TEST(
        adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInStreamModeWithCorrectSizedArray);
    RUN_TEST(
        adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInStreamModeWithWrongSizedArray);
    RUN_TEST(
        adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInFifoModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInFifoModeWithWrongSizedArray);
    RUN_TEST(
        adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInTriggerModeWithCorrectSizedArray);
    RUN_TEST(
        adxl345bGetMeasurementsForNMillisecondsReadCorrectValuesInTriggerModeWithWrongSizedArray);

    RUN_TEST(adxl345bConvertDataXYZCorrectValue);

    return UNITY_END();
}
