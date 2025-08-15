#include "I2cUnitTest.h"
#include "eai/sensor/Adxl345b.h"
#include "eai/sensor/Adxl345bTypedefs.h"
#include <memory.h>
#include <stdlib.h>
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

static uint32_t checkIfDataIsZero(adxl345bRawData_t *samples, uint8_t expectetRawData);
static uint32_t checkIfDataIsZero(adxl345bRawData_t *samples, uint8_t expectetRawData) {
    uint32_t counter = 0;
    for (int i = 0; i < numberOfSamples; i++) {
        for (int j = 0; j < ADXL345B_SIZE_OF_RAWDATA; j++) {
            if (samples[i].data[j] == 0) {
                counter++;
            } else {
                printf("expected: %b, ", expectetRawData);
                printf("actual: %b \n", samples[i].data[j]);
            }
        }
    }
    return counter;
}

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

    adxl345bGetSingleMeasurement(sensor, &rawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(byteZero, rawData.data, ADXL345B_SIZE_OF_RAWDATA);
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

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInStreamMode(void) {
    adxl345bRawData_t samples[numberOfSamples];
    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    uint32_t counter = checkIfDataIsZero(samples, expectedRawData);
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInFifoMode(void) {
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    uint32_t counter = checkIfDataIsZero(samples, expectedRawData);
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInTriggerMode(void) {
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL();
    }

    uint32_t counter = checkIfDataIsZero(samples, expectedRawData);
    TEST_ASSERT_EQUAL(0, counter);
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
    uint8_t expectedRawData = byteTwo;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

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
                                 numberOfSamples * ADXL345B_SIZE_OF_RAWDATA);
}

void adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInFifoMode(void) {
    microseconds = 800;

    /*generate Array for Data*/

    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    if (errorCode != ADXL345B_PARM_ERROR) {
        TEST_FAIL();
    }
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples,
                                 numberOfSamples * ADXL345B_SIZE_OF_RAWDATA);
}

void adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInTriggerMode(void) {
    microseconds = 1000;

    /*generate Array for Data*/

    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteThree;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    if (errorCode != ADXL345B_PARM_ERROR) {
        TEST_FAIL();
    }
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples,
                                 numberOfSamples * ADXL345B_SIZE_OF_RAWDATA);
}

/* endregion GetMeasurementsForNmicrosecondsReadCorrectValues */
/* endregion adxl345bGetMeasurementsForNMicroseconds */

void adxl345bConvertDataXYZCorrectValueWithFullResON(void) {
    /* test assumes that 2G Full Range is the used Range */
    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        printf("adxl345bChangeMeasurementRange failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

    float expected_xAxis = 0, expected_yAxis = 0, expected_zAxis = 0;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    const uint8_t MSB_MASK = 0b00000011;
    // const float SCALE_FACTOR_FOR_RANGE = 0.00391; //ten-bit
    const float SCALE_FACTOR_FOR_RANGE = 0.00377f;
    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
    uint8_t topByte = byteZero & MSB_MASK;

    /* set rawData */
    adxl345bRawData_t rawData;

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
    errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    for (int i = 0; i < ADXL345B_SIZE_OF_RAWDATA; i++) {
        printf("%b \n", rawData.data[i]);
    }
    if (errorCode != ADXL345B_NO_ERROR) {
        printf("adxl345bGetSingleMeasurement failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bWithFullResON;
    errorCode =
        adxl345bConvertDataXYZ(sensor, &actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL();
    }

    printf("%b \n", actual_xAxis);
    printf("%b \n", actual_yAxis);
    printf("%b \n", actual_zAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_xAxis, actual_xAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_yAxis, actual_yAxis);
    TEST_ASSERT_EQUAL_FLOAT(expected_zAxis, actual_zAxis);
}

void adxl345bConvertDataXYZCorrectValueWithFullResOFF(void) {
    /* test assumes that 2G 10Bit Range is the used Range */
    adxl345bErrorCode_t errorCode = adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        printf("adxl345bChangeMeasurementRange failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

    float expected_xAxis = 0, expected_yAxis = 0, expected_zAxis = 0;
    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
    const uint8_t MSB_MASK = 0b00000011;
    // const float SCALE_FACTOR_FOR_RANGE = 0.00391; //ten-bit
    const float SCALE_FACTOR_FOR_RANGE = 0.00377f;
    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
    uint8_t topByte = byteZero & MSB_MASK;

    /* set rawData */
    adxl345bRawData_t rawData;

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
    errorCode = adxl345bGetSingleMeasurement(sensor, &rawData);
    for (int i = 0; i < ADXL345B_SIZE_OF_RAWDATA; i++) {
        printf("%b \n", rawData.data[i]);
    }
    if (errorCode != ADXL345B_NO_ERROR) {
        printf("adxl345bGetSingleMeasurement failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bWithFullResOFF;
    errorCode =
        adxl345bConvertDataXYZ(sensor, &actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL();
    }

    printf("%b \n", actual_xAxis);
    printf("%b \n", actual_yAxis);
    printf("%b \n", actual_zAxis);
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

    // ChangeMeasurementRange
    RUN_TEST(adxl345bChangeMeasurementRangeGetSendCommandFail_errorIfHardwarFails);
    RUN_TEST(adxl345bChangeMeasurementRangeGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bChangeMeasurementRangeGetReceiveDataFail_errorIfHardwarFails);
    RUN_TEST(adxl345bChangeMeasurementRangeGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bChangeMeasurementRangeWrongRange);
    RUN_TEST(adxl345bChangeMeasurementRangeChangeSuccessful);

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

    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerMode);

    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInStreamMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInFifoMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInTriggerMode);

    // GetMeasurementsForNmicroseconds
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsGetReceiveDataFail_errorIfAckMissing);

    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInBypassMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInStreamMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInFifoMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInTriggerMode);

    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInStreamMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInFifoMode);
    RUN_TEST(adxl345bGetMeasurementsForNMicrosecondsReadCorrectValuesInTriggerMode);

    RUN_TEST(adxl345bConvertDataXYZCorrectValueWithFullResON);
    RUN_TEST(adxl345bConvertDataXYZCorrectValueWithFullResOFF);
    return UNITY_END();
}
