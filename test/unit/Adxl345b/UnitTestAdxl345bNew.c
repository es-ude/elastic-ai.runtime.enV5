#include "Adxl345b.h"
#include "I2c.h"
#include "Sleep.h"
#include "Time.h"
#include "unity.h"

adxl345bSensorConfiguration_t sensor;

uint64_t timeUs64(void) {
    return 1;
}

uint32_t timeUs32(void) {
    return 1;
}

void sleep_for_ms(uint32_t msToSleep) {}

void sleep_for_us(uint64_t usToSleep) {}

i2cErrorCode_t i2cWriteError;

i2cErrorCode_t i2cReadError;

i2cErrorCode_t i2cWriteCommand(i2c_inst_t *hostAddress, uint8_t slaveAddress,
                               const uint8_t *commandBuffer, uint16_t sizeOfCommandBuffer) {
    return i2cWriteError;
}

i2cErrorCode_t i2cReadData(i2c_inst_t *hostAddress, uint8_t slaveAddress, uint8_t *readBuffer,
                           uint8_t sizeOfReadBuffer) {
    return i2cReadError;
}

void setUp(void) {
    /* Default: Point to Pass */
    i2cWriteError = I2C_NO_ERROR;
    i2cReadError = I2C_NO_ERROR;
    adxl345bChangeMeasurementRange(sensor, ADXL345B_2G_RANGE);
}

void tearDown(void) {}

/* region adxl345bReadSerialNumber */

void adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwareFails(void) {
    uint8_t serialNumber;
    i2cWriteError = I2C_INIT_ERROR;
    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing(void) {
    uint8_t serialNumber;
    i2cWriteError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwareFails(void) {
    uint8_t serialNumber;
    i2cReadError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing(void) {
    uint8_t serialNumber;
    i2cReadError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bReadSerialNumberReadSuccessful(void) {
    uint8_t serialNumber;

    uint8_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

// START HERE: does this test make sense?
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
    uint8_t rawData[6];
    i2cWriteError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing(void) {
    uint8_t rawData[6];
    i2cWriteError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails(void) {
    uint8_t rawData[6];
    i2cReadError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing(void) {
    uint8_t rawData[6];
    i2cReadError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetSingleMeasurementReadSuccessful(void) {
    uint8_t rawData[6];
    adxl345bErrorCode_t errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

// void adxl345bGetSingleMeasurementReadCorrectValue(void) {
//     uint8_t sizeOfRawData = 6;
//     uint8_t rawData[sizeOfRawData];
//
//     adxl345bGetSingleMeasurement(sensor, rawData);
//     for(int i = 0; i < sizeOfRawData; i ++){
//         TEST_ASSERT_EQUAL_UINT8(byteZero, rawData[i]);
//     }
//
// }

/* endregion adxl345bGetSingleMeasurement */

/* region adxl345bGetMultipleMeasurements */

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 1;
    uint8_t *samples[numberOfSamples];

    i2cWriteError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 16;
    uint8_t *samples[numberOfSamples];
    i2cWriteError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 32;
    uint8_t *samples[numberOfSamples];
    i2cReadError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 33;
    uint8_t *samples[numberOfSamples];
    i2cReadError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMultipleMeasurementsReadSuccessful(void) {
    uint32_t numberOfSamples = 50;
    uint8_t sizeOfRawData = 6;
    uint8_t *samples[numberOfSamples];

    adxl345bErrorCode_t errorCode =
        adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

// void adxl345bGetMultipleMeasurementsReadCorrectValues(void) {
//     /*generate Array for Data*/
//         uint8_t sizeOfRawData = 6;
//         uint32_t numberOfSamples = 5;
//         uint8_t *samples[numberOfSamples];
//
//         /* change ReadCommands to generate expected raw data received from I2C*/
//         uint8_t expectedRawData = byteZero;
//         for(int readMode = 0; readMode < 3 ; readMode++) {
//             switch (readMode) {
//                 case 0: //use default StreamMode
//                     break;
//                 case 1:
//                     i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
//                     expectedRawData = byteOne;
//                     break;
//                 case 2:
//                     i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
//                     expectedRawData = byteTwo;
//                     break;
//             }
//             adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);
//
//
//             for (int i = 0; i < numberOfSamples; i++) {
//                 for (int j = 0; j < sizeOfRawData; j++) {
//                     TEST_ASSERT_EQUAL_UINT8(expectedRawData, samples[i][j]);
//                 }
//             }
//         }
// }

/* endregion adxl345bGetMultipleMeasurements */

/* region adxl345bGetMeasurementsForNSeconds */

void adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 50;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    uint32_t seconds = 3;

    i2cWriteError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &numberOfSamples);

    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 16;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    uint32_t seconds = 3;
    i2cWriteError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_SEND_COMMAND_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfHardwareFails(void) {
    uint32_t numberOfSamples = 32;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    uint32_t seconds = 3;
    i2cReadError = I2C_INIT_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfAckMissing(void) {
    uint32_t numberOfSamples = 33;
    uint8_t sizeOfRawData = 6;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    uint32_t seconds = 3;
    i2cReadError = I2C_ACK_ERROR;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_RECEIVE_DATA_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNSecondsReadSuccessful(void) {
    uint32_t numberOfSamples = 1;
    uint8_t sizeOfRawData = 6;
    uint32_t seconds = 3;
    uint8_t samples[numberOfSamples * sizeOfRawData];
    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMilliseconds(sensor, samples, seconds, &numberOfSamples);
    TEST_ASSERT_EQUAL_UINT8(ADXL345B_NO_ERROR, errorCode);
}

// void adxl345bGetMeasurementsForNSecondsReadCorrectValues(void) {
//     /*generate Array for Data*/
//     uint8_t sizeOfRawData = 6;
//     uint32_t numberOfSamples = 5;
//     uint32_t seconds = 3;
//     uint8_t *samples[numberOfSamples];

//    /* change ReadCommands to generate expected raw data received from I2C*/
//    uint8_t expectedRawData = byteZero;
//    for(int readMode = 0; readMode < 3 ; readMode++) {
//        switch (readMode) {
//            case 0: //use default StreamMode
//                break;
//            case 1:
//                i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInFifoMode;
//                expectedRawData = byteOne;
//                break;
//            case 2:
//                i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
//                expectedRawData = byteTwo;
//                break;
//        }
//        adxl345bGetMeasurementsForNSeconds(sensor, samples, seconds, numberOfSamples);
//
//
//        for (int i = 0; i < numberOfSamples; i++) {
//            for (int j = 0; j < sizeOfRawData; j++) {
//                TEST_ASSERT_EQUAL_UINT8(expectedRawData, samples[i][j]);
//            }
//        }
//    }
//}
///* endregion adxl345bGetMeasurementsForNMilliseconds */
//
//
//
// void adxl345bConvertDataXYZCorrectValue(void) {
//    /* test assumes that 2G Full Range is the used Range */
//    i2cUnittestReadCommand = i2cUnittestReadCommandPassForAdxl345bInTriggerMode;
//
//    float expected_xAxis = 0, expected_yAxis = 0, expected_zAxis = 0;
//    float actual_xAxis = 0, actual_yAxis = 0, actual_zAxis = 0;
//    const uint8_t MSB_MASK = 0b00000011;
//    const float SCALE_FACTOR_FOR_RANGE = 0.0043f;
//    /* only used lower 2 bits -> 2G Range consists of 10 Bit*/
//    uint8_t topByte = byteTwo & MSB_MASK;
//
//    /* set rawData */
//    uint8_t sizeOfRawData = 6;
//    uint8_t rawData[sizeOfRawData];
//    for(int i = 0; i < sizeOfRawData; i ++){
//        rawData[i] = byteTwo;
//    }
//
//    /* fill expected with random generated */
//    if (topByte <= (MSB_MASK >> 1)) {
//        /* CASE: positive value */
//        int rawValue = (int)(((uint16_t)(topByte & MSB_MASK) << 8) | (uint16_t)byteTwo);
//        float realValue = (float)rawValue * SCALE_FACTOR_FOR_RANGE;
//        expected_xAxis = expected_yAxis = expected_zAxis = realValue;
//    } else {
//        /* CASE: negative value
//         *
//         * 1. revert 10 bit two complement
//         * -> number-1 and Flip least 9 bits
//         * 2. convert to float value
//         * 3. multiply with (-1)
//         * 4. multiply with scale factor
//         */
//        uint16_t rawValue = ((uint16_t)(topByte & (MSB_MASK >> 1)) << 8) | (uint16_t)byteTwo;
//        rawValue = (rawValue - 0x0001) ^ (((MSB_MASK >> 1) << 8) | 0x00FF);
//        float realValue = (-1) * (float)rawValue * SCALE_FACTOR_FOR_RANGE;
//        expected_xAxis = expected_yAxis = expected_zAxis = realValue;
//    }
//    printf("actual vorher: %f %f %f ",actual_xAxis, actual_yAxis, actual_zAxis);
//
//    adxl345bConvertDataXYZ(&actual_xAxis, &actual_yAxis, &actual_zAxis, rawData);
//    printf("actual nachher:%f %f %f ",actual_xAxis, actual_yAxis, actual_zAxis);
//    TEST_ASSERT_EQUAL_FLOAT(expected_xAxis, actual_xAxis);
//    TEST_ASSERT_EQUAL_FLOAT(expected_yAxis, actual_yAxis);
//    TEST_ASSERT_EQUAL_FLOAT(expected_zAxis, actual_zAxis);
//}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(adxl345bReadSerialNumberGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bReadSerialNumberGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadSerialNumberGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bReadSerialNumberGetReceiveDataFail_errorIfAckMissing);
    RUN_TEST(adxl345bReadSerialNumberReadSuccessful);
    RUN_TEST(adxl345bReadSerialNumberReadCorrectValue); //currently fails

    RUN_TEST(adxl345bGetSingleMeasurementGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetSingleMeasurementGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetSingleMeasurementGetReceiveDataFail_errorIfAckMissing);
    //RUN_TEST(adxl345bGetSingleMeasurementReadSuccessful); //does not terminate
    //RUN_TEST(adxl345bGetSingleMeasurementReadCorrectValue); //needs to be rewritten

    RUN_TEST(adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMultipleMeasurementsGetReceiveDataFail_errorIfAckMissing);
    //RUN_TEST(adxl345bGetMultipleMeasurementsReadSuccessful); //numerical error
    //RUN_TEST(adxl345bGetMultipleMeasurementsReadCorrectValues); //needs to be rewritten

    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetSendCommandFail_errorIfAckMissing);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfHardwareFails);
    RUN_TEST(adxl345bGetMeasurementsForNSecondsGetReceiveDataFail_errorIfAckMissing);
    //RUN_TEST(adxl345bGetMeasurementsForNSecondsReadSuccessful); //does not terminate
    //RUN_TEST(adxl345bGetMeasurementsForNSecondsReadCorrectValues); //needs to be rewritten

    //RUN_TEST(adxl345bConvertDataXYZCorrectValue); //needs to be rewritten

    return UNITY_END();
}
