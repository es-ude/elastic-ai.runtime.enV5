#include "Sht3x.h"
#include "I2cUnitTest.h"
#include "unity.h"

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPassForSht3x;
    i2cUnittestReadCommand = i2cUnittestReadCommandPassForSht3x;
}

void tearDown(void) {}

/* region SHT3X_ReadStatusRegister */

void sht3xReadStatusRegisterGetSendCommandFailErrorIfHardwareFails(void) {
    sht3xStatusRegister_t statusRegister;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xReadStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xReadStatusRegisterGetSendCommandFailErrorIfAckMissing(void) {
    sht3xStatusRegister_t statusRegister;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xReadStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xReadStatusRegisterGetReceiveDataFailErrorIfHardwareFails(void) {
    sht3xStatusRegister_t statusRegister;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xReadStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xReadStatusRegisterGetReceiveDataFailErrorIfAckMissing(void) {
    sht3xStatusRegister_t statusRegister;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xReadStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xReadStatusRegisterGetChecksumFailError(void) {
    sht3xStatusRegister_t statusRegister;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFailForSht3x;

    sht3xErrorCode_t errorCode = sht3xReadStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void sht3xReadStatusRegisterReadSuccessful(void) {
    sht3xStatusRegister_t statusRegister;

    sht3xErrorCode_t errorCode = sht3xReadStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void sht3xReadStatusRegisterReadCorrectValue(void) {
    sht3xStatusRegister_t expected_statusRegister, actual_statusRegister;

    /* fill expected with random generated */
    expected_statusRegister.config = (byteZero << 8) | (byteOne & 0xFF);

    sht3xReadStatusRegister(&actual_statusRegister);
    TEST_ASSERT_EQUAL_UINT16(expected_statusRegister.config, actual_statusRegister.config);
}

/* endregion*/
/* region SHT3X_ReadSerialNumber */

void sht3xReadSerialNumberGetSendCommandFailErrorIfHardwareFails(void) {
    uint32_t serialNumber;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xReadSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xReadSerialNumberGetSendCommandFailErrorIfAckMissing(void) {
    uint32_t serialNumber;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xReadSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xReadSerialNumberGetReceiveDataFailErrorIfHardwareFails(void) {
    uint32_t serialNumber;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xReadSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xReadSerialNumberGetReceiveDataFailErrorIfAckMissing(void) {
    uint32_t serialNumber;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xReadSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xReadSerialNumberGetChecksumFailError(void) {
    uint32_t serialNumber;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFailForSht3x;

    sht3xErrorCode_t errorCode = sht3xReadSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void sht3xReadSerialNumberReadSuccessful(void) {
    uint32_t serialNumber;

    sht3xErrorCode_t errorCode = sht3xReadSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void sht3xReadSerialNumberReadCorrectValue(void) {
    uint32_t expected_serialNumber, actual_serialNumber;

    /* fill expected with random generated */
    expected_serialNumber = (byteZero << 24) | (byteOne << 16) | (byteZero << 8) | byteOne;

    sht3xReadSerialNumber(&actual_serialNumber);
    TEST_ASSERT_EQUAL_UINT32(expected_serialNumber, actual_serialNumber);
}

/* endregion */
/* region SHT3X_GetTemperature */

void sht3xGetTemperatureGetSendCommandFailErrorIfHardwareFails(void) {
    float temperature;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xGetTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xGetTemperatureGetSendCommandFailErrorIfAckMissing(void) {
    float temperature;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xGetTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xGetTemperatureGetReceiveDataFailErrorIfHardwareFails(void) {
    float temperature;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xGetTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xGetTemperatureGetChecksumFailError(void) {
    float temperature;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFailForSht3x;

    sht3xErrorCode_t errorCode = sht3xGetTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void sht3xGetTemperatureGetReceiveDataFailErrorIfAckMissing(void) {
    float temperature;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xGetTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xGetTemperatureReadSuccessful(void) {
    float temperature;

    sht3xErrorCode_t errorCode = sht3xGetTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void sht3xGetTemperatureReadCorrectValue(void) {
    float expected_temperature, actual_temperature;

    /* fill expected with random generated */
    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature = 175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) - 45.0f;

    sht3xGetTemperature(&actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
}

/* endregion */
/* region SHT3X_GetHumidity */

void sht3xGetHumidityGetSendCommandFailErrorIfHardwareFails(void) {
    float humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xGetHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xGetHumidityGetSendCommandFailErrorIfAckMissing(void) {
    float humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xGetHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xGetHumidityGetReceiveDataFailErrorIfHardwareFails(void) {
    float humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xGetHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xGetHumidityGetReceiveDataFailErrorIfAckMissing(void) {
    float humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xGetHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xGetHumidityGetChecksumFailError(void) {
    float humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFailForSht3x;

    sht3xErrorCode_t errorCode = sht3xGetHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void sht3xGetHumidityReadSuccessful(void) {
    float humidity;

    uint8_t err = sht3xGetHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, err);
}

void sht3xGetHumidityReadCorrectValue(void) {
    float expected_humidity, actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity = 100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    sht3xGetHumidity(&actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_GetTemperatureAndHumidity  */

void sht3xGetTemperatureAndHumidityGetSendCommandFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xGetTemperatureAndHumidityGetSendCommandFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xGetTemperatureAndHumidityGetReceiveDataFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xGetTemperatureAndHumidityGetReceiveDataFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xGetTemperatureAndHumidityGetChecksumFailError(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFailForSht3x;

    sht3xErrorCode_t errorCode = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void sht3xGetTemperatureAndHumidityReadSuccessful(void) {
    float temperature, humidity;

    uint8_t err = sht3xGetTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, err);
}

void sht3xGetTemperatureAndHumidityReadCorrectValue(void) {
    float expected_temperature, actual_temperature, expected_humidity, actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity = 100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature = 175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) - 45.0f;

    sht3xGetTemperatureAndHumidity(&actual_temperature, &actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_ReadMeasurementBuffer  */

void sht3xReadMeasurementBufferGetSendCommandFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xReadMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xReadMeasurementBufferGetSendCommandFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xReadMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xReadMeasurementBufferGetReceiveDataFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xReadMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xReadMeasurementBufferGetReceiveDataFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xReadMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void sht3xReadMeasurementBufferGetChecksumFailError(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFailForSht3x;

    sht3xErrorCode_t errorCode = sht3xReadMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void sht3xReadMeasurementBufferReadSuccessful(void) {
    float temperature, humidity;

    sht3xErrorCode_t errorCode = sht3xReadMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void sht3xReadMeasurementBufferReadCorrectValue(void) {
    float expected_temperature, actual_temperature, expected_humidity, actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity = 100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature = 175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) - 45.0f;

    sht3xReadMeasurementBuffer(&actual_temperature, &actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_EnableHeater  */

void sht3xEnableHeaterGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xEnableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xEnableHeaterGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xEnableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

/* endregion */
/* region SHT3X_DisableHeater  */

void sht3xDisableHeaterGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xDisableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xDisableHeaterGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xDisableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

/* endregion */
/* region SHT3X_SoftReset  */

void sht3xSoftResetGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    sht3xErrorCode_t errorCode = sht3xSoftReset();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void sht3xSoftResetGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    sht3xErrorCode_t errorCode = sht3xSoftReset();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(sht3xReadStatusRegisterGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xReadStatusRegisterGetSendCommandFailErrorIfAckMissing);
    RUN_TEST(sht3xReadStatusRegisterGetReceiveDataFailErrorIfHardwareFails);
    RUN_TEST(sht3xReadStatusRegisterGetReceiveDataFailErrorIfAckMissing);
    RUN_TEST(sht3xReadStatusRegisterGetChecksumFailError);
    RUN_TEST(sht3xReadStatusRegisterReadSuccessful);
    RUN_TEST(sht3xReadStatusRegisterReadCorrectValue);

    RUN_TEST(sht3xReadSerialNumberGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xReadSerialNumberGetSendCommandFailErrorIfAckMissing);
    RUN_TEST(sht3xReadSerialNumberGetReceiveDataFailErrorIfHardwareFails);
    RUN_TEST(sht3xReadSerialNumberGetReceiveDataFailErrorIfAckMissing);
    RUN_TEST(sht3xReadSerialNumberGetChecksumFailError);
    RUN_TEST(sht3xReadSerialNumberReadSuccessful);
    RUN_TEST(sht3xReadSerialNumberReadCorrectValue);

    RUN_TEST(sht3xGetTemperatureGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xGetTemperatureGetSendCommandFailErrorIfAckMissing);
    RUN_TEST(sht3xGetTemperatureGetReceiveDataFailErrorIfHardwareFails);
    RUN_TEST(sht3xGetTemperatureGetReceiveDataFailErrorIfAckMissing);
    RUN_TEST(sht3xGetTemperatureGetChecksumFailError);
    RUN_TEST(sht3xGetTemperatureReadSuccessful);
    RUN_TEST(sht3xGetTemperatureReadCorrectValue);

    RUN_TEST(sht3xGetHumidityGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xGetHumidityGetSendCommandFailErrorIfAckMissing);
    RUN_TEST(sht3xGetHumidityGetReceiveDataFailErrorIfHardwareFails);
    RUN_TEST(sht3xGetHumidityGetReceiveDataFailErrorIfAckMissing);
    RUN_TEST(sht3xGetHumidityGetChecksumFailError);
    RUN_TEST(sht3xGetHumidityReadSuccessful);
    RUN_TEST(sht3xGetHumidityReadCorrectValue);

    RUN_TEST(sht3xGetTemperatureAndHumidityGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xGetTemperatureAndHumidityGetSendCommandFailErrorIfAckMissing);
    RUN_TEST(sht3xGetTemperatureAndHumidityGetReceiveDataFailErrorIfHardwareFails);
    RUN_TEST(sht3xGetTemperatureAndHumidityGetReceiveDataFailErrorIfAckMissing);
    RUN_TEST(sht3xGetTemperatureAndHumidityGetChecksumFailError);
    RUN_TEST(sht3xGetTemperatureAndHumidityReadSuccessful);
    RUN_TEST(sht3xGetTemperatureAndHumidityReadCorrectValue);

    RUN_TEST(sht3xReadMeasurementBufferGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xReadMeasurementBufferGetSendCommandFailErrorIfAckMissing);
    RUN_TEST(sht3xReadMeasurementBufferGetReceiveDataFailErrorIfHardwareFails);
    RUN_TEST(sht3xReadMeasurementBufferGetReceiveDataFailErrorIfAckMissing);
    RUN_TEST(sht3xReadMeasurementBufferGetChecksumFailError);
    RUN_TEST(sht3xReadMeasurementBufferReadSuccessful);
    RUN_TEST(sht3xReadMeasurementBufferReadCorrectValue);

    RUN_TEST(sht3xEnableHeaterGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xEnableHeaterGetSendCommandFailErrorIfAckMissing);

    RUN_TEST(sht3xDisableHeaterGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xDisableHeaterGetSendCommandFailErrorIfAckMissing);

    RUN_TEST(sht3xSoftResetGetSendCommandFailErrorIfHardwareFails);
    RUN_TEST(sht3xSoftResetGetSendCommandFailErrorIfAckMissing);

    return UNITY_END();
}
