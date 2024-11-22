#include "Adxl345b.h"
#include "I2cUnitTest.h"
#include <stdlib.h>
#include <unity.h>

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost
 */

adxl345bSensorConfiguration_t sensor;

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForAdxl345b;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;

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
    expected_serialNumber = 0b10011111;

    adxl345bReadSerialNumber(sensor, &actual_serialNumber);
    TEST_ASSERT_EQUAL_UINT8(expected_serialNumber, actual_serialNumber);
}

/* endregion adxl345bReadSerialNumber */

/* region adxl345bGetSingleMeasurement */

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfHardwareFails(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadSuccessful(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadCorrectValue(void) {
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRawData; i++) {
        rawData[i] = 0;
    }

    adxl345bGetSingleMeasurement(sensor, rawData);
    for (int i = 0; i < sizeOfRawData; i++) {
        TEST_ASSERT_EQUAL_UINT8(byteZero, rawData[i]);
    }
}
/* endregion adxl345bGetSingleMeasurement */

/* region adxl345bGetMultipleMeasurements */

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 1;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];

    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 16;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 33;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}


void adxl345bGetMultipleMeasurementsReadSuccessful(void) {
    uint32_t numberOfSamples = 50;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}


//readBuffer[0] = 0b10000000 | 0b00000010 |
//0b00011111;
//checkIfSomeDataIsZero
//faengt aktuell nicht bei 0,1 und 2 ab!
//ausschliesslich Default  (Test)Mode kann stream trigger und fifo
void adxl345bGetMultipleMeasurementsCheckOverwritingDataInStreamMode(void) {
    uint32_t numberOfSamples = 1;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    //check if some data is 0
    uint8_t counter = 0;
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i += sizeOfRawData) {
        if (samples[i] == 0) {
            counter++;
            printf("samples %b counter %i \n", samples[i], counter);

        } else {
            printf("should be 10011111 but is: %b \n", samples[i]);
            counter = 0;
        }
    }
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInFifoMode(void) {
    uint32_t numberOfSamples = 1;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    //check if some data is 0
    uint8_t counter = 0;
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i += sizeOfRawData) {
        if (samples[i] == 0) {
            counter++;
            printf("samples %b counter %i \n", samples[i], counter);

        } else {
            printf("should be 10011111 but is: %b \n", samples[i]);
            counter = 0;
        }
    }
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsCheckOverwritingDataInTriggerMode(void) {
    uint32_t numberOfSamples = 1;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("If this ever occurs and no other test fails, please write test for it.");
    }

    //check if some data is 0
    uint8_t counter = 0;
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i += sizeOfRawData) {
        if (samples[i] == 0) {
            counter++;
            printf("samples %b counter %i \n", samples[i], counter);

        } else {
            printf("should be 10011111 but is: %b \n", samples[i]);
            counter = 0;
        }
    }
    TEST_ASSERT_EQUAL(0, counter);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint32_t numberOfSamples = 31;
    uint8_t sizeOfRawData = 6;
    uint8_t wrongSize = 1;
    uint8_t samples[numberOfSamples * sizeOfRawData + wrongSize];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData + wrongSize); i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData + wrongSize);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[numberOfSamples * sizeOfRawData], wrongSize);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint32_t numberOfSamples = 31;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples * sizeOfRawData);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfRawData = 6;
    uint8_t wrongSize = 5;
    uint8_t samples[numberOfSamples * sizeOfRawData + wrongSize];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData + wrongSize); i++) {
        samples[i] = 0;
    }

    /* no change of ReadCommands needed to generate expected raw data received from I2C*/
    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData + wrongSize);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[numberOfSamples * sizeOfRawData], wrongSize);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (int i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    /* no change of ReadCommands needed to generate expected raw data received from I2C*/
    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples * sizeOfRawData);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithWrongSizedArray(void) {
    /*generate Array for Data*/
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfRawData = 6;
    uint8_t wrongSize = 3;
    uint8_t samples[numberOfSamples * sizeOfRawData + wrongSize];
    //ensure all Data is 0
    for (size_t i = 0; i < (numberOfSamples * sizeOfRawData + wrongSize); i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteTwo;

    uint8_t expectedArray[numberOfSamples * sizeOfRawData];
    uint8_t samplessss[numberOfSamples * sizeOfRawData + wrongSize];
    for (int i = 0; i < numberOfSamples * sizeOfRawData; i++) {
        expectedArray[i] = expectedRawData;
    }
    for (size_t i = 0; i < numberOfSamples * sizeOfRawData + wrongSize; i++) {
        printf("before %zu: expected Array %u - ", i, expectedArray[i]);
        printf("samples Array %u\n", samples[i]);
        samplessss[i] = samples[i];
    }
    printf("before: sizeOfArray %u\n", numberOfSamples * sizeOfRawData + wrongSize);
    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData + wrongSize);

    for (size_t i = 0; i < numberOfSamples * sizeOfRawData + wrongSize; i++) {
        printf("after %zu : expected Array 0x%02x - ", i, expectedArray[i]);
        printf("samples Array 0x%02x - ", samples[i]);
        printf("samples before Array 0x%02x\n", samplessss[i]);
        if (i == 45 | i == 46 || i == 47) {
            printf(" Element %zu ist %d ", i, samples[i]);
        }
    }
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples * sizeOfRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, &samples[numberOfSamples * sizeOfRawData], wrongSize);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithCorrectSizedArray(void) {
    /*generate Array for Data*/
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    //ensure all Data is 0
    for (size_t i = 0; i < (numberOfSamples * sizeOfRawData); i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteTwo;

    uint8_t expectedArray[numberOfSamples * sizeOfRawData];
    uint8_t samplessss[numberOfSamples * sizeOfRawData];
    for (int i = 0; i < numberOfSamples * sizeOfRawData; i++) {
        expectedArray[i] = expectedRawData;
    }
    for (size_t i = 0; i < numberOfSamples * sizeOfRawData; i++) {
        printf("before %zu: expected Array %u - ", i, expectedArray[i]);
        printf("samples Array %u\n", samples[i]);
        samplessss[i] = samples[i];
    }
    uint32_t length = numberOfSamples * sizeOfRawData;
    printf("before: sizeOfArray %u\n", numberOfSamples * sizeOfRawData);
    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples * sizeOfRawData);

    for (size_t i = 0; i < numberOfSamples * sizeOfRawData; i++) {
        printf("after %zu : expected Array 0x%02x - ", i, expectedArray[i]);
        printf("samples Array 0x%02x - ", samples[i]);
        printf("samples before Array 0x%02x\n", samplessss[i]);
        if (i == 45 | i == 46 || i == 47) {
            printf(" Element %zu ist %d ", i, samples[i]);
        }
    }

    for (size_t i = 0; i < numberOfSamples * sizeOfRawData; i += 6) {
        TEST_ASSERT_EQUAL_UINT8(expectedRawData, samples[i]);
    }
    //expectedArray
    //for (size_t i = 0; i < numberOfSamples * sizeOfRawData; i += 6) {
    //    TEST_ASSERT_EQUAL_UINT_ARRAY(expectedArray, samples, length);
    //}
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, length);
}

/* endregion adxl345bGetMultipleMeasurements */

/* region adxl345bGetMeasurementsForNSeconds */

void adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 50;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    uint32_t seconds = 3;

    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);

    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 16;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    uint32_t seconds = 3;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    uint32_t seconds = 3;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 33;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    uint32_t seconds = 3;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);

}

void adxl345bGetMeasurementsForNSecondsReadSuccessfulInFifoMode(void) {
    uint32_t numberOfSamples = 5;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }
    uint32_t seconds = 1;

    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;

    adxl345bErrorCode_t errorCode =
            adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}


void adxl345bGetMeasurementsForNSecondsReadSuccessfulInStreamMode(void) {
    uint32_t numberOfSamples = 5;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }
    uint32_t seconds = 1;

    adxl345bErrorCode_t errorCode = adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds,
                                                                            &sizeOfRequestedRawData);

    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);

}

void adxl345bGetMeasurementsForNSecondsReadSuccessfulInTriggerMode(void) {
    uint32_t numberOfSamples = 5;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }
    uint32_t seconds = 1;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;

        adxl345bErrorCode_t errorCode =
                adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);
        TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);

}

// 3 different tests
void adxl345bGetMeasurementsForNSecondsReadCorrectValuesInStreamMode(void) {
    uint32_t seconds = 1;

    /*generate Array for Data*/
    uint32_t numberOfSamples = 2;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInStreamMode;
    uint8_t expectedRawData = byteZero;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);
    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
}


void adxl345bGetMeasurementsForNSecondsReadCorrectValuesInFifoMode(void) {
    uint32_t seconds = 1;

    /*generate Array for Data*/
    uint32_t numberOfSamples = 2;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }
    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);

}

void adxl345bGetMeasurementsForNSecondsReadCorrectValuesInTriggerMode(void) {
    uint32_t seconds = 1;

    /*generate Array for Data*/
    uint32_t numberOfSamples = 2;
    uint8_t sizeOfSingleRawData = 6;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfSingleRawData;
    uint8_t samples[sizeOfRequestedRawData];
    //ensure all Data is 0
    for (int i = 0; i < sizeOfRequestedRawData; i++) {
        samples[i] = 0;
    }

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
    uint8_t expectedRawData = byteTwo;

    adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &sizeOfRequestedRawData);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, sizeOfRequestedRawData);
}

/* endregion adxl345bGetMeasurementsForNSeconds */

void adxl345bConvertDataXYZCorrectValue(void) {
    /* test assumes that 2G Full Range is the used Range */

    float expected_xAxis = byteOne, expected_yAxis = byteOne, expected_zAxis = byteOne;
    float actual_xAxis = byteOne, actual_yAxis = byteOne, actual_zAxis = byteOne;
    const uint8_t MSB_MASK = 0b00000011;
    const float SCALE_FACTOR_FOR_RANGE = 0.0043f;
    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
    uint8_t topByte = byteTwo & MSB_MASK;

    /* set rawData */
    uint8_t sizeOfRawData = 6;
    uint8_t rawData[sizeOfRawData];
    for (int i = 0; i < sizeOfRawData; i++) {
        rawData[i] = byteTwo;
    }

    /* fill expected with random generated */
    if (topByte <= (MSB_MASK >> 1)) {
        /* CASE: positive value */
        int rawValue = (int) (((uint16_t)(topByte & MSB_MASK) << 8) | (uint16_t) byteTwo);
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
        uint16_t rawValue = ((uint16_t)(topByte & (MSB_MASK >> 1)) << 8) | (uint16_t) byteTwo;
        rawValue = (rawValue - 0x0001) ^ (((MSB_MASK >> 1) << 8) | 0x00FF);
        float realValue = (-1) * (float) rawValue * SCALE_FACTOR_FOR_RANGE;
        expected_xAxis = expected_yAxis = expected_zAxis = realValue;
    }
    printf("actual vorher: %f %f %f ", actual_xAxis, actual_yAxis, actual_zAxis);

    adxl345bConvertDataXYZ(&actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);
    printf("actual nachher:%f %f %f ", actual_xAxis, actual_yAxis, actual_zAxis);
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
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithCorrectSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoModeWithWrongSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInStreamModeWithWrongSizedArray);
    RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerModeWithWrongSizedArray);
    
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInStreamMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInFifoMode);
    RUN_TEST(adxl345bGetMultipleMeasurementsCheckOverwritingDataInTriggerMode);

    //StreamMode
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsReadSuccessfulInStreamMode);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsReadCorrectValuesInStreamMode);

    //FifoMode
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsReadSuccessfulInFifoMode);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsReadCorrectValuesInFifoMode);

    //TriggerMode
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsReadSuccessfulInTriggerMode);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsReadCorrectValuesInTriggerMode);

    //RUN_TEST(adxl345bConvertDataXYZCorrectValue);

    return UNITY_END();
}
