#include "I2cUnitTest.h"
#include "eai/sensor/Adxl345b.h"
#include "eai/sensor/Adxl345bTypedefs.h"
#include <memory.h>
#include <stdlib.h>
#include <unity.h>

/*! JUST HERE TO SATISFY THE COMPILER
 *
 * @param i2cHost

/* region I2C_ReadCommandPass */
/* region Header_I2C_ReadCommandPass */

i2cErrorCode_t i2cUnittestReadCommandPassInBypassMode(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                      uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassInStreamMode(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                      uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassInFifoMode(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassInTriggerMode(uint8_t *readBuffer,
                                                       uint8_t sizeOfReadBuffer,
                                                       uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassWithFullResON(uint8_t *readBuffer,
                                                       uint8_t sizeOfReadBuffer,
                                                       uint8_t slaveAddress, i2c_inst_t *i2cHost);

i2cErrorCode_t i2cUnittestReadCommandPassWithFullResOFF(uint8_t *readBuffer,
                                                        uint8_t sizeOfReadBuffer,
                                                        uint8_t slaveAddress, i2c_inst_t *i2cHost);
/* endregion Header_I2C_ReadCommandPass */

i2cErrorCode_t i2cUnittestReadCommandPassInBypassMode(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                      uint8_t slaveAddress, i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);

    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassInStreamMode(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                      uint8_t slaveAddress, i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    if (sizeOfReadBuffer == 1) {
        readBuffer[0] = 0b10000000 | 0b00000010 |
                        0b00011111; // set Stream Mode, Watermark and samples for Trigger
        return 0x00;
    }
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteOne);
    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassInFifoMode(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                                    uint8_t slaveAddress, i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    if (sizeOfReadBuffer == 1) {
        readBuffer[0] = 0b01000000 | 0b00000010 |
                        0b00011111; // set FiFo Mode, Watermark and samples for Trigger
        return 0x00;
    }
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);
    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassInTriggerMode(uint8_t *readBuffer,
                                                       uint8_t sizeOfReadBuffer,
                                                       uint8_t slaveAddress, i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    if (sizeOfReadBuffer == 1) {
        readBuffer[0] = 0b11000000 | 0b00000010 |
                        0b00011111; // set Trigger Mode, Watermark and samples for Trigger
        return 0x00;
    }
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, byteZero);
    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassWithFullResOFF(uint8_t *readBuffer,
                                                        uint8_t sizeOfReadBuffer,
                                                        uint8_t slaveAddress, i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, dataFormatFullResOFF);

    return 0x00;
}

i2cErrorCode_t i2cUnittestReadCommandPassWithFullResON(uint8_t *readBuffer,
                                                       uint8_t sizeOfReadBuffer,
                                                       uint8_t slaveAddress, i2c_inst_t *i2cHost) {

    /* generate sample data without any real world connection to test
     * implementation */
    i2cUnittestWriteByteMultipleTimes(readBuffer, sizeOfReadBuffer, dataFormatFullResON);

    return 0x00;
}

/* endregion ReadCommandPass */

adxl345bSensorConfiguration_t sensor;
uint32_t numberOfSamples;
uint8_t serialNumber;
uint32_t microseconds;

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPass;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInBypassMode;

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
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInStreamMode;
    uint8_t expectedRawData = byteOne;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInFiFoMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInFifoMode;
    uint8_t expectedRawData = byteZero;

    adxl345bGetMultipleMeasurements(sensor, samples, numberOfSamples);

    TEST_ASSERT_EACH_EQUAL_UINT8(expectedRawData, samples, numberOfSamples);
}

void adxl345bGetMultipleMeasurementsReadCorrectValuesInTriggerMode(void) {
    /*generate Array for Data*/
    adxl345bRawData_t samples[numberOfSamples];

    /* change ReadCommands to generate expected raw data received from I2C*/
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInTriggerMode;
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

    i2cUnittestReadCommand = i2cUnittestReadCommandPassInStreamMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);

    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_PARM_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInFifoMode(void) {
    adxl345bRawData_t samples[numberOfSamples];
    microseconds = 40;

    i2cUnittestReadCommand = i2cUnittestReadCommandPassInFifoMode;

    adxl345bErrorCode_t errorCode =
        adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, samples, numberOfSamples);
    /* we expect a parameter error, because we use DUMMY-TIME-FACADE-LIB */
    TEST_ASSERT_EQUAL_HEX8(ADXL345B_PARM_ERROR, errorCode);
}

void adxl345bGetMeasurementsForNMicrosecondsReadSuccessfulInTriggerMode(void) {
    adxl345bRawData_t samples[numberOfSamples];

    microseconds = 50;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInTriggerMode;

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
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInStreamMode;
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
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInFifoMode;
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
    i2cUnittestReadCommand = i2cUnittestReadCommandPassInTriggerMode;
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

    i2cUnittestReadCommand = i2cUnittestReadCommandPassWithFullResON;
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

    i2cUnittestReadCommand = i2cUnittestReadCommandPassWithFullResOFF;
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
