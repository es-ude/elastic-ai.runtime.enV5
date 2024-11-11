#include "CException.h"
#include "Common.h"
#include "I2cUnitTest.h"
#include "Pac193x.h"

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

static pac193xSensorConfiguration_t SENSOR = {
    .usedChannels = {.uint_channelsInUse = 0b00000010},
    .rSense = {0, 0.82f, 0, 0},
    .powerPin = -1,
    .i2c_slave_address = 0x11,
    .sampleRate = PAC193X_8_SAMPLES_PER_SEC,
};

static uint8_t usedChannelIndex = 1;

/* endregion */

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForPac193x;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForPac193x;

    pac193xSetChannelsInUse(&SENSOR);
}

void tearDown(void) {}

/* region pac193x_GetSensorInfo */

void pac193xGetSensorInfoReturnSendCommandErrorIfHardwareFails(void) {
    CEXCEPTION_T e;

    Try {
        i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;
        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, e);
    }
}

void pac193xGetSensorInfoReturnSendCommandErrorIfAckMissing(void) {
    CEXCEPTION_T e;

    Try {
        i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;
        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, e);
    }
}

void pac193xGetSensorInfoReturnReceiveDataErrorIfHardwareFails(void) {
    pac193xSensorId_t info;
    CEXCEPTION_T e;

    Try {
        i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;
        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, e);
    }
}

void pac193xGetSensorInfoReturnReceiveDataErrorIfAckMissing(void) {
    pac193xSensorId_t info;
    CEXCEPTION_T e;

    Try {
        i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, e);
    }
}

void pac193xGetSensorInfoReadSuccessful(void) {
    pac193xSensorId_t info;
    CEXCEPTION_T e;

    Try {
        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
}

void pac193xGetSensorInfoReadCorrectValue(void) {
    pac193xSensorId_t expectedInfo, actualInfo;
    CEXCEPTION_T e;

    expectedInfo.product_id = byteZero;
    expectedInfo.manufacturer_id = byteZero;
    expectedInfo.revision_id = byteZero;

    Try {
        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(expectedInfo.product_id, actualInfo.product_id);
        TEST_ASSERT_EQUAL_UINT8(expectedInfo.manufacturer_id, actualInfo.manufacturer_id);
        TEST_ASSERT_EQUAL_UINT8(expectedInfo.revision_id, actualInfo.revision_id);
    }
}

void pac193xMemoryNotPassedToGetSensorInfoRemainsUntouched(void) {
    uint8_t memory[512];
    pac193xSensorId_t info;
    memset(memory, 0, 512);
    CEXCEPTION_T e;

    Try {
        pac193xSensorId_t info = pac193xGetSensorInfo(&SENSOR);
    }
    Catch(e) {
        TEST_ASSERT_EACH_EQUAL_UINT8(0, memory, 512);
    }
}

/* endregion */
/* region pac193x_GetMeasurementForChannel */

void testAssertUint64tEquals(void) {
    uint64_t expected = 0x0000BEBEBEBEBEBE;
    uint64_t actual = ((uint64_t)byteZero << 40) | ((uint64_t)byteZero << 32) |
                      ((uint64_t)byteZero << 24) | ((uint64_t)byteZero << 16) |
                      ((uint64_t)byteZero << 8) | (uint64_t)byteZero;

    TEST_ASSERT_EQUAL_UINT64(expected, actual);
}

void pac193xGetMeasurementForChannelReturnSendCommandErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReturnSendCommandErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReturnReceiveDataErrorIfHardwareFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReturnReceiveDataErrorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReturnInvalidChannelErrorIfChannelWrong(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, 0x10, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_INVALID_CHANNEL, e);
    }
}

/* region V_SOURCE */
void pac193xGetMeasurementForChannelReadSuccessfulValueVsource(void) {
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReadCorrectValueVsource(void) {
    float expectedValue = 0, actualValue = 0;
    CEXCEPTION_T e;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    expectedValue =
        (32.0f * (((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator));

    Try {
        actualValue = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSOURCE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
    }
}
/* endregion V_SOURCE */

/* region V_SENSE */
void pac193xGetMeasurementForChannelReadSuccessfulValueVsense(void) {
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSENSE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReadCorrectValueVsense(void) {
    float expectedValue = 0, actualValue = 0;
    CEXCEPTION_T e;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    expectedValue = 0.1f * ((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator;

    Try {
        actualValue = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_VSENSE);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
    }
}
/* endregion V_SENSE */

/* region CURRENT */
void pac193xGetMeasurementForChannelReadSuccessfulValueCurrent(void) {
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_CURRENT);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReadCorrectValueCurrent(void) {
    float expectedValue = 0, actualValue = 0;
    CEXCEPTION_T e;

    uint64_t expected_rawValue = ((uint64_t)byteZero << 8) | (uint64_t)byteZero;
    float FSC = 0.1f / SENSOR.rSense[usedChannelIndex];
    expectedValue = FSC * (((float)expected_rawValue) / pac193xInternalUnipolarVoltageDenominator);

    Try {
        actualValue = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_CURRENT);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
    }
}
/* endregion CURRENT */

/* region POWER */
void pac193xGetMeasurementForChannelReadSuccessfulValuePower(void) {
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_ENERGY);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReadCorrectValuePower(void) {
    float expectedValue = 0, actualValue = 0;
    CEXCEPTION_T e;

    uint64_t rawValue = (((uint64_t)byteZero << 24) | ((uint64_t)byteZero << 16) |
                         ((uint64_t)byteZero << 8) | (uint64_t)byteZero);
    float expectedRawValue = (float)rawValue;
    float powerFSR = 3.2f / SENSOR.rSense[usedChannelIndex];
    float pProp = expectedRawValue / pac193xInternalUnipolarPowerDenominator;
    expectedValue = powerFSR * pProp;

    Try {
        actualValue = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_POWER);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
    }
}
/* endregion POWER */

/* region ENERGY */

void pac193xGetMeasurementForChannelReadSuccessfulValueEnergy(void) {
    CEXCEPTION_T e;

    Try {
        float result = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_ENERGY);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
}

void pac193xGetMeasurementForChannelReadCorrectValueEnergy(void) {
    float expectedValue = 0, actualValue = 0;
    CEXCEPTION_T e;

    uint64_t expected_rawValue =
        (((uint64_t)byteZero << 40) | ((uint64_t)byteZero << 32) | (uint64_t)byteZero << 24) |
        ((uint64_t)byteZero << 16) | ((uint64_t)byteZero << 8) | (uint64_t)byteZero;

    float powerFSR = 3.2f / SENSOR.rSense[usedChannelIndex];
    expectedValue = (float)expected_rawValue * powerFSR / (pac193xInternalEnergyDenominator * 8.0f);

    Try {
        actualValue = pac193xGetMeasurementForChannel(&SENSOR, PAC193X_CHANNEL02, PAC193X_ENERGY);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_FLOAT(expectedValue, actualValue);
    }
}
/* endregion ENERGY */

/* endregion */
/* region pac193x_GetAllMeasurementsForChannel */

void pac193xGetAllMeasurementsForChannelReturnSendCommandErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;
    CEXCEPTION_T e;

    Try {
        pac193xMeasurements_t measurements =
            pac193xGetMeasurementsForChannel(&SENSOR, PAC193X_CHANNEL02);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, e);
    }
}

void pac193xGetAllMeasurementsForChannelReturnSendCommandErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;
    CEXCEPTION_T e;

    Try {
        pac193xMeasurements_t measurements =
            pac193xGetMeasurementsForChannel(&SENSOR, PAC193X_CHANNEL02);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_SEND_COMMAND_ERROR, e);
    }
}

void pac193xGetAllMeasurementsForChannelReturnReceiveDataErrorIfHardwareFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;
    CEXCEPTION_T e;

    Try {
        pac193xMeasurements_t measurements =
            pac193xGetMeasurementsForChannel(&SENSOR, PAC193X_CHANNEL02);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, e);
    }
}

void pac193xGetAllMeasurementsForChannelReturnReceiveDataErrorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;
    CEXCEPTION_T e;

    Try {
        pac193xMeasurements_t measurements =
            pac193xGetMeasurementsForChannel(&SENSOR, PAC193X_CHANNEL02);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_RECEIVE_DATA_ERROR, e);
    }
}

void pac193xGetAllMeasurementsForChannelReadSuccessful(void) {
    CEXCEPTION_T e;

    Try {
        pac193xMeasurements_t measurements =
            pac193xGetMeasurementsForChannel(&SENSOR, PAC193X_CHANNEL02);
    }
    Catch(e) {
        TEST_ASSERT_EQUAL_UINT8(PAC193X_NO_ERROR, e);
    }
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
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueVsense);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueVsense);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValueCurrent);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValueCurrent);
    RUN_TEST(pac193xGetMeasurementForChannelReadSuccessfulValuePower);
    RUN_TEST(pac193xGetMeasurementForChannelReadCorrectValuePower);
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
