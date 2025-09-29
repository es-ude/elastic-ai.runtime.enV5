#include "CException.h"
#include "unity.h"

#include "I2cUnitTest.h"
#include "eai/sensor/Sht3x.h"

/* region I2C_ReadCommands*/
/* region HEADER_I2C_ReadCommands*/
i2cErrorCode_t i2cUnittestReadCommandPass(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                          uint8_t slaveAddress, i2c_inst_t *i2cHost);
i2cErrorCode_t i2cUnittestReadCommandProvokeChecksumFail(uint8_t *readBuffer,
                                                         uint8_t sizeOfReadBuffer,
                                                         uint8_t slaveAddress, i2c_inst_t *i2cHost);
/* endregion HEADER_I2C_ReadCommands*/

i2cErrorCode_t i2cUnittestReadCommandPass(uint8_t *readBuffer, uint8_t sizeOfReadBuffer,
                                          uint8_t slaveAddress, i2c_inst_t *i2cHost) {
    /**
     * @brief failing I²C implementation
     *
     * I2C only writes one byte,
     * this implementation breaks this rule by always writing three bytes in a
     * row
     *
     * FIXME: Improve implementation to match I2C specifications
     */

    /* generate sample data without any real world connection to test
     * implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index = index + 3) {
        readBuffer[index] = byteZero;
        readBuffer[index + 1] = byteOne;
        readBuffer[index + 2] = correctByteChecksum;
    }

    return 0x00;
}
i2cErrorCode_t i2cUnittestReadCommandProvokeChecksumFail(uint8_t *readBuffer,
                                                         uint8_t sizeOfReadBuffer,
                                                         uint8_t slaveAddress,
                                                         i2c_inst_t *i2cHost) {
    /**
     * @brief failing I2C implementation
     *
     * I2C only writes one byte,
     * this implementation breaks this rule by always writing three bytes in a
     * row
     *
     * FIXME: Improve implementation to match I2C specifications
     */

    /* generate sample data without any real world connection to test
     * implementation */
    for (uint8_t index = 0; index < sizeOfReadBuffer; index = index + 3) {
        readBuffer[index] = byteZero;
        readBuffer[index + 1] = byteOne;
        readBuffer[index + 2] = wrongByteChecksum;
    }

    return 0x00;
}
/* endregion I2C_ReadCommands */

static sht3xSensorConfiguration_t sensor;

void setUp(void) {
    /* Default: Point to Pass */
    i2cUnittestWriteCommand = i2cUnittestWriteCommandPass;
    i2cUnittestReadCommand = i2cUnittestReadCommandPass;
}

void tearDown(void) {}

/* region SHT3X_ReadStatusRegister */

void sht3xReadStatusRegisterGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;
    CEXCEPTION_T exception;

    Try {
        sht3xReadStatusRegister(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadStatusRegisterGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;
    CEXCEPTION_T exception;

    Try {
        sht3xReadStatusRegister(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadStatusRegisterGetReceiveDataFailErrorIfHardwareFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;
    CEXCEPTION_T exception;

    Try {
        sht3xReadStatusRegister(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadStatusRegisterGetReceiveDataFailErrorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;
    CEXCEPTION_T exception;

    Try {
        sht3xReadStatusRegister(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadStatusRegisterGetChecksumFailError(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFail;
    CEXCEPTION_T exception;

    Try {
        sht3xReadStatusRegister(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadStatusRegisterReadSuccessful(void) {
    CEXCEPTION_T exception;

    Try {
        sht3xReadStatusRegister(sensor);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

void sht3xReadStatusRegisterReadCorrectValue(void) {
    /* fill expected with random generated */
    sht3xStatusRegister_t expected_statusRegister = {.config = (byteZero << 8) | (byteOne & 0xFF)};
    CEXCEPTION_T exception;

    Try {
        sht3xStatusRegister_t actual_statusRegister = sht3xReadStatusRegister(sensor);
        TEST_ASSERT_EQUAL_UINT16(expected_statusRegister.config, actual_statusRegister.config);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

/* endregion*/
/* region SHT3X_ReadSerialNumber */

void sht3xReadSerialNumberGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;
    CEXCEPTION_T exception;

    Try {
        sht3xReadSerialNumber(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadSerialNumberGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;
    CEXCEPTION_T exception;

    Try {
        sht3xReadSerialNumber(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadSerialNumberGetReceiveDataFailErrorIfHardwareFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;
    CEXCEPTION_T exception;

    Try {
        sht3xReadSerialNumber(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadSerialNumberGetReceiveDataFailErrorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;
    CEXCEPTION_T exception;

    Try {
        sht3xReadSerialNumber(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadSerialNumberGetChecksumFailError(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFail;
    CEXCEPTION_T exception;

    Try {
        sht3xReadSerialNumber(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadSerialNumberReadSuccessful(void) {
    CEXCEPTION_T exception;

    Try {
        sht3xReadSerialNumber(sensor);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

void sht3xReadSerialNumberReadCorrectValue(void) {
    /* fill expected with random generated */
    uint32_t expected_serialNumber = (byteZero << 24) | (byteOne << 16) | (byteZero << 8) | byteOne;
    CEXCEPTION_T exception;

    Try {
        uint32_t actual_serialNumber = sht3xReadSerialNumber(sensor);
        TEST_ASSERT_EQUAL_UINT32(expected_serialNumber, actual_serialNumber);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

/* endregion */
/* region SHT3X_GetTemperature */

void sht3xGetTemperatureGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperature(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperature(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureGetReceiveDataFailErrorIfHardwareFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperature(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureGetChecksumFailError(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFail;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperature(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureGetReceiveDataFailErrorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperature(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureReadSuccessful(void) {
    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperature(sensor);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

void sht3xGetTemperatureReadCorrectValue(void) {
    /* fill expected with random generated */
    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    float expected_temperature =
        175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) - 45.0f;

    CEXCEPTION_T exception;
    Try {
        float actual_temperature = sht3xGetTemperature(sensor);
        TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

/* endregion */
/* region SHT3X_GetHumidity */

void sht3xGetHumidityGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xGetHumidity(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetHumidityGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xGetHumidity(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetHumidityGetReceiveDataFailErrorIfHardwareFails(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xGetHumidity(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetHumidityGetReceiveDataFailErrorIfAckMissing(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xGetHumidity(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetHumidityGetChecksumFailError(void) {
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFail;

    CEXCEPTION_T exception;
    Try {
        sht3xGetHumidity(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetHumidityReadSuccessful(void) {
    CEXCEPTION_T exception;
    Try {
        sht3xGetHumidity(sensor);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

void sht3xGetHumidityReadCorrectValue(void) {
    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    float expected_humidity = 100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    CEXCEPTION_T exception;
    Try {
        float actual_humidity = sht3xGetHumidity(sensor);
        TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

/* endregion */
/* region SHT3X_GetTemperatureAndHumidity  */

void sht3xGetTemperatureAndHumidityGetSendCommandFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureAndHumidityGetSendCommandFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureAndHumidityGetReceiveDataFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureAndHumidityGetReceiveDataFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureAndHumidityGetChecksumFailError(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFail;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xGetTemperatureAndHumidityReadSuccessful(void) {
    float temperature, humidity;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

void sht3xGetTemperatureAndHumidityReadCorrectValue(void) {
    float expected_temperature, actual_temperature, expected_humidity, actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity = 100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature = 175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) - 45.0f;

    CEXCEPTION_T exception;
    Try {
        sht3xGetTemperatureAndHumidity(sensor, &actual_temperature, &actual_humidity);
        TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
        TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

/* endregion */
/* region SHT3X_ReadMeasurementBuffer  */

void sht3xReadMeasurementBufferGetSendCommandFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xReadMeasurementBuffer(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadMeasurementBufferGetSendCommandFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xReadMeasurementBuffer(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadMeasurementBufferGetReceiveDataFailErrorIfHardwareFails(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xReadMeasurementBuffer(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadMeasurementBufferGetReceiveDataFailErrorIfAckMissing(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xReadMeasurementBuffer(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadMeasurementBufferGetChecksumFailError(void) {
    float temperature, humidity;
    i2cUnittestReadCommand = i2cUnittestReadCommandProvokeChecksumFail;

    CEXCEPTION_T exception;
    Try {
        sht3xReadMeasurementBuffer(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xReadMeasurementBufferReadSuccessful(void) {
    float temperature, humidity;

    CEXCEPTION_T exception;
    Try {
        sht3xReadMeasurementBuffer(sensor, &temperature, &humidity);
    }
    Catch(exception) {
        TEST_FAIL();
    }
}

void sht3xReadMeasurementBufferReadCorrectValue(void) {
    float expected_temperature, actual_temperature, expected_humidity, actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity = 100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature = 175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) - 45.0f;

    sht3xReadMeasurementBuffer(sensor, &actual_temperature, &actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_EnableHeater  */

void sht3xEnableHeaterGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xEnableHeater(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xEnableHeaterGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xEnableHeater(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

/* endregion */
/* region SHT3X_DisableHeater  */

void sht3xDisableHeaterGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xDisableHeater(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xDisableHeaterGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xDisableHeater(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

/* endregion */
/* region SHT3X_SoftReset  */

void sht3xSoftResetGetSendCommandFailErrorIfHardwareFails(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandHardwareDefect;

    CEXCEPTION_T exception;
    Try {
        sht3xSoftReset(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_HARDWARE_ERROR, exception);
        return;
    }
    TEST_FAIL();
}

void sht3xSoftResetGetSendCommandFailErrorIfAckMissing(void) {
    i2cUnittestWriteCommand = i2cUnittestWriteCommandAckMissing;

    CEXCEPTION_T exception;
    Try {
        sht3xSoftReset(sensor);
    }
    Catch(exception) {
        TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, exception);
        return;
    }
    TEST_FAIL();
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
