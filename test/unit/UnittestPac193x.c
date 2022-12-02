#include "Pac193x.h"
#include "i2c/I2cUnitTest.h"
#include <stdint.h>
#include <string.h>
#include <unity.h>

/* region CONSTANTS */

/*! Denominator for unipolar voltage measurement: 2^{16} = 65536 */
static const float pac193xInternalUnipolarVoltageDenominator = (float)(1U << 16);

/*! Denominator for unipolar power measurement: 2^{32} = 4294967296
 *
 * \Information This denominator is 2^{28} according to the datasheet,
 *            however testing has shown that 2^{32} is actually correct
 */
static const float pac193xInternalUnipolarPowerDenominator = (float)(1ULL << 32);

/*! Denominator for energy measurement: 2^28 = 268435456 */
static const float pac193xInternalEnergyDenominator = (float)(1ULL << 28);

/*! rate for samples per second#
 *
 * \Important Must be set using the ctrl register */
static const float pac193xInternalSamplingRate = 1024;

static pac193xSensorConfiguration_t SENSOR = {
    .usedChannels = {.uint_channelsInUse = 0b00000010},
    .rSense = {0, 0.82f, 0, 0},
    .powerPin = -1,
    .i2c_slave_address = 0x11,
};

static uint8_t usedChannelIndex = 1;

/* endregion */

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForPac193x;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForPac193x;

    pac193xSetChannelsInUse(SENSOR);
}

void tearDown(void) {}

/* region pac193x_GetSensorInfo */

void pac193xGetSensorInfoReturnSendCommandErrorIfHardwareFails(void) {
    pac193xSensorId_t info;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    pac193xErrorCode_t errorCode = pac193xGetSensorInfo(SENSOR, &info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
}

void pac193xGetSensorInfoReturnSendCommandErrorIfAckMissing(void) {
    pac193xSensorId_t info;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    pac193xErrorCode_t errorCode = pac193xGetSensorInfo(SENSOR, &info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
}

void pac193xGetSensorInfoReturnReceiveDataErrorIfHardwareFails(void) {
    pac193xSensorId_t info;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    pac193xErrorCode_t errorCode = pac193xGetSensorInfo(SENSOR, &info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
}

void pac193xGetSensorInfoReturnReceiveDataErrorIfAckMissing(void) {
    pac193xSensorId_t info;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    pac193xErrorCode_t errorCode = pac193xGetSensorInfo(SENSOR, &info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
}

void pac193xGetSensorInfoReadSuccessful(void) {
    pac193xSensorId_t info;

    pac193xErrorCode_t errorCode = pac193xGetSensorInfo(SENSOR, &info);
    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetSensorInfoReadCorrectValue(void) {
    pac193xSensorId_t expectedInfo, actualInfo;

    expectedInfo.product_id = byteZero;
    expectedInfo.manufacturer_id = byteZero;
    expectedInfo.revision_id = byteZero;

    pac193xGetSensorInfo(SENSOR, &actualInfo);

    TEST_ASSERT_EQUAL_UINT8(expectedInfo.product_id, actualInfo.product_id);
    TEST_ASSERT_EQUAL_UINT8(expectedInfo.manufacturer_id, actualInfo.manufacturer_id);
    TEST_ASSERT_EQUAL_UINT8(expectedInfo.revision_id, actualInfo.revision_id);
}

void pac193xMemoryNotPassedToGetSensorInfoRemainsUntouched(void) {
    uint8_t memory[512];
    pac193xSensorId_t info;

    memset(memory, 0, 512);

    pac193xGetSensorInfo(SENSOR, &info);

    TEST_ASSERT_EACH_EQUAL_UINT8(0, memory, 512);
}

/* endregion */
/* region pac193x_GetMeasurementForChannel */

void pac193xGetMeasurementForChannelReturnSendCommandErrorIfHardwareFails(void) {
    float result;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReturnSendCommandErrorIfAckMissing(void) {
    float result;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReturnReceiveDataErrorIfHardwareFails(void) {
    float result;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReturnReceiveDataErrorIfAckMissing(void) {
    float result;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReturnInvalidChannelErrorIfChannelWrong(void) {
    float result;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, 0x10, PAC193X_VSOURCE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_INVALID_CHANNEL, errorCode);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueVsource(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueVsource(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    expectedValue =
        (32.0f * (((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator));

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueVsourceAvr(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE_AVG, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueVsourceAvr(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    expectedValue =
        (32.0f * (((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator));

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE_AVG, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueVsense(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSENSE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueVsense(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    expectedValue = 0.1f * ((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator;

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSENSE, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueVsenseAvg(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSENSE_AVG, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueVsenseAvg(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    expectedValue = 0.1f * ((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator;

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_VSENSE_AVG, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueIsense(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_ISENSE, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueIsense(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    float FSC = 0.1f / SENSOR.rSense[usedChannelIndex];
    expectedValue = FSC * (((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator);

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_ISENSE, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueIsenseAvg(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_ISENSE_AVG, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueIsenseAvg(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    float FSC = 0.1f / SENSOR.rSense[usedChannelIndex];
    expectedValue = FSC * (((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator);

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_ISENSE_AVG, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValuePactual(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_PACTUAL, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValuePactual(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t rawValue = (((uint64_t)byteZero << 24) | ((uint64_t)byteZero << 16) |
                         ((uint64_t)byteZero << 8) | (uint64_t)byteZero);
    float expectedRawValue = (float)rawValue;
    float powerFSR = 3.2f / SENSOR.rSense[usedChannelIndex];
    float pProp = expectedRawValue / pac193xInternalUnipolarPowerDenominator;
    expectedValue = powerFSR * pProp;

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_PACTUAL, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void pac193xGetMeasurementForChannelReadSuccessfulValueEnergy(void) {
    float result;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_ENERGY, &result);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

void pac193xGetMeasurementForChannelReadCorrectValueEnergy(void) {
    float expectedValue = 0, actualValue = 0;

    uint64_t rawValue = ((uint64_t)byteZero << 40) | ((uint64_t)byteZero << 32) |
                        ((uint64_t)byteZero << 24) | ((uint64_t)byteZero << 16) |
                        ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    float expectedRawValue = (float)rawValue;
    float powerFSR = 3.2f / SENSOR.rSense[usedChannelIndex];
    expectedValue = expectedRawValue * powerFSR /
                    (pac193xInternalEnergyDenominator * pac193xInternalSamplingRate);

    pac193xGetMeasurementForChannel(SENSOR, PAC193X_CHANNEL02, PAC193X_ENERGY, &actualValue);

    TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
}

void testAssertUint64tEquals(void) {
    uint64_t expected = 0x0000BEBEBEBEBEBE;
    uint64_t actual = ((uint64_t)byteZero << 40) | ((uint64_t)byteZero << 32) |
                      ((uint64_t)byteZero << 24) | ((uint64_t)byteZero << 16) |
                      ((uint64_t)byteZero << 8) | (uint64_t)byteZero;

    TEST_ASSERT_EQUAL_UINT64(expected, actual);
}

/* endregion */
/* region pac193x_GetAllMeasurementsForChannel */

void pac193xGetAllMeasurementsForChannelReturnSendCommandErrorIfHardwareFails(void) {
    pac193xMeasurements_t measurements;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(SENSOR, PAC193X_CHANNEL02, &measurements);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
}

void pac193xGetAllMeasurementsForChannelReturnSendCommandErrorIfAckMissing(void) {
    pac193xMeasurements_t measurements;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(SENSOR, PAC193X_CHANNEL02, &measurements);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, errorCode);
}

void pac193xGetAllMeasurementsForChannelReturnReceiveDataErrorIfHardwareFails(void) {
    pac193xMeasurements_t measurements;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(SENSOR, PAC193X_CHANNEL02, &measurements);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
}

void pac193xGetAllMeasurementsForChannelReturnReceiveDataErrorIfAckMissing(void) {
    pac193xMeasurements_t measurements;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(SENSOR, PAC193X_CHANNEL02, &measurements);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, errorCode);
}

void pac193xGetAllMeasurementsForChannelReadSuccessful(void) {
    pac193xMeasurements_t measurements;

    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(SENSOR, PAC193X_CHANNEL02, &measurements);

    TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, errorCode);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(pac193xMemoryNotPassedToGetSensorInfoRemainsUntouched);
    RUN_TEST(pac193xGetSensorInfoReturnSendCommandErrorIfHardwareFails);
    RUN_TEST(pac193xGetSensorInfoReturnSendCommandErrorIfAckMissing);
    RUN_TEST(pac193xGetSensorInfoReturnReceiveDataErrorIfHardwareFails);
    RUN_TEST(pac193xGetSensorInfoReturnReceiveDataErrorIfAckMissing);
    RUN_TEST(pac193xGetSensorInfoReadSuccessful);
    RUN_TEST(pac193xGetSensorInfoReadCorrectValue);

    RUN_TEST(pac193xGetMeasurementForChannelReturnSendCommandErrorIfHardwareFails);
    RUN_TEST(pac193xGetMeasurementForChannelReturnSendCommandErrorIfAckMissing);
    RUN_TEST(pac193xGetMeasurementForChannelReturnReceiveDataErrorIfHardwareFails);
    RUN_TEST(pac193xGetMeasurementForChannelReturnReceiveDataErrorIfAckMissing);
    RUN_TEST(pac193xGetMeasurementForChannelReturnInvalidChannelErrorIfChannelWrong);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueVsource);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueVsource);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueVsourceAvr);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueVsourceAvr);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueVsense);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueVsense);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueVsenseAvg);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueVsenseAvg);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueIsense);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueIsense);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueIsenseAvg);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueIsenseAvg);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValuePactual);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValuePactual);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueEnergy);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueEnergy);

    RUN_TEST(testAssertUint64tEquals);

    RUN_TEST(pac193xGetAllMeasurementsForChannelReturnSendCommandErrorIfHardwareFails);
    RUN_TEST(pac193xGetAllMeasurementsForChannelReturnSendCommandErrorIfAckMissing);
    RUN_TEST(pac193xGetAllMeasurementsForChannelReturnReceiveDataErrorIfHardwareFails);
    RUN_TEST(pac193xGetAllMeasurementsForChannelReturnReceiveDataErrorIfAckMissing);
    RUN_TEST(pac193xGetAllMeasurementsForChannelReadSuccessful);
    UnityPrint("Not testing for correct values. Values generated by "
               "pac193x_GetMeasurementForChannel(...) and have been tested before.");

    return UNITY_END();
}
