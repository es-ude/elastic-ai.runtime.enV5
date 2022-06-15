#define TEST_BUILD

//#include "i2c/i2c.h"
#include "i2c/i2c_test.h"
#include "sht3x_public.h"
#include "unity.h"

void setUp(void) {
    /* Default: Point to Pass */
    I2C_WriteCommand_ptr = I2C_WriteCommand_Pass_for_SHT3X;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Pass_for_SHT3X;
}

void tearDown(void) {}

/* region SHT3X_ReadStatusRegister */

void SHT3X_ReadStatusRegister_get_SEND_COMMAND_FAIL_error_if_hardware_fails(
    void) {
    sht3x_statusRegister statusRegister;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_readStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_ReadStatusRegister_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    sht3x_statusRegister statusRegister;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_readStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_ReadStatusRegister_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(
    void) {
    sht3x_statusRegister statusRegister;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_readStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_ReadStatusRegister_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    sht3x_statusRegister statusRegister;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_readStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_ReadStatusRegister_get_CHECKSUM_FAIL_error(void) {
    sht3x_statusRegister statusRegister;
    I2C_ReadCommand_ptr = I2C_ReadCommand_provoke_checksum_fail_for_SHT3X;

    sht3x_errorCode errorCode = sht3x_readStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void SHT3X_ReadStatusRegister_read_successful(void) {
    sht3x_statusRegister statusRegister;

    sht3x_errorCode errorCode = sht3x_readStatusRegister(&statusRegister);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void SHT3X_ReadStatusRegister_read_correct_value(void) {
    sht3x_statusRegister expected_statusRegister, actual_statusRegister;

    /* fill expected with random generated */
    expected_statusRegister.config = (byteZero << 8) | (byteOne & 0xFF);

    sht3x_readStatusRegister(&actual_statusRegister);
    TEST_ASSERT_EQUAL_UINT16(expected_statusRegister.config,
                             actual_statusRegister.config);
}

/* endregion*/
/* region SHT3X_ReadSerialNumber */

void SHT3X_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_hardware_fails(
    void) {
    uint32_t serialNumber;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    uint32_t serialNumber;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(
    void) {
    uint32_t serialNumber;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    uint32_t serialNumber;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_ReadSerialNumber_get_CHECKSUM_FAIL_error(void) {
    uint32_t serialNumber;
    I2C_ReadCommand_ptr = I2C_ReadCommand_provoke_checksum_fail_for_SHT3X;

    sht3x_errorCode errorCode = sht3x_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void SHT3X_ReadSerialNumber_read_successful(void) {
    uint32_t serialNumber;

    sht3x_errorCode errorCode = sht3x_readSerialNumber(&serialNumber);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void SHT3X_ReadSerialNumber_read_correct_value(void) {
    uint32_t expected_serialNumber, actual_serialNumber;

    /* fill expected with random generated */
    expected_serialNumber =
        (byteZero << 24) | (byteOne << 16) | (byteZero << 8) | byteOne;

    sht3x_readSerialNumber(&actual_serialNumber);
    TEST_ASSERT_EQUAL_UINT32(expected_serialNumber, actual_serialNumber);
}

/* endregion */
/* region SHT3X_GetTemperature */

void SHT3X_GetTemperature_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    float temperature;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_getTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_GetTemperature_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    float temperature;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_getTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_GetTemperature_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    float temperature;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_getTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_GetTemperature_get_CHECKSUM_FAIL_error(void) {
    float temperature;
    I2C_ReadCommand_ptr = I2C_ReadCommand_provoke_checksum_fail_for_SHT3X;

    sht3x_errorCode errorCode = sht3x_getTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void SHT3X_GetTemperature_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    float temperature;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_getTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_GetTemperature_read_successful(void) {
    float temperature;

    sht3x_errorCode errorCode = sht3x_getTemperature(&temperature);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void SHT3X_GetTemperature_read_correct_value(void) {
    float expected_temperature, actual_temperature;

    /* fill expected with random generated */
    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature =
        175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) -
        45.0f;

    sht3x_getTemperature(&actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
}

/* endregion */
/* region SHT3X_GetHumidity */

void SHT3X_GetHumidity_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    float humidity;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_getHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_GetHumidity_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    float humidity;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_getHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_GetHumidity_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(void) {
    float humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_getHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_GetHumidity_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(void) {
    float humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_getHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_GetHumidity_get_CHECKSUM_FAIL_error(void) {
    float humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_provoke_checksum_fail_for_SHT3X;

    sht3x_errorCode errorCode = sht3x_getHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void SHT3X_GetHumidity_read_successful(void) {
    float humidity;

    uint8_t err = sht3x_getHumidity(&humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, err);
}

void SHT3X_GetHumidity_read_correct_value(void) {
    float expected_humidity, actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity =
        100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    sht3x_getHumidity(&actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_GetTemperatureAndHumidity  */

void SHT3X_GetTemperatureAndHumidity_get_SEND_COMMAND_FAIL_error_if_hardware_fails(
    void) {
    float temperature, humidity;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode =
        sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_GetTemperatureAndHumidity_get_SEND_COMMAND_FAIL_error_if_ACK_missing(
    void) {
    float temperature, humidity;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode =
        sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_GetTemperatureAndHumidity_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(
    void) {
    float temperature, humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    sht3x_errorCode errorCode =
        sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_GetTemperatureAndHumidity_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(
    void) {
    float temperature, humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    sht3x_errorCode errorCode =
        sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_GetTemperatureAndHumidity_get_CHECKSUM_FAIL_error(void) {
    float temperature, humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_provoke_checksum_fail_for_SHT3X;

    sht3x_errorCode errorCode =
        sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void SHT3X_GetTemperatureAndHumidity_read_successful(void) {
    float temperature, humidity;

    uint8_t err = sht3x_getTemperatureAndHumidity(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, err);
}

void SHT3X_GetTemperatureAndHumidity_read_correct_value(void) {
    float expected_temperature, actual_temperature, expected_humidity,
        actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity =
        100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature =
        175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) -
        45.0f;

    sht3x_getTemperatureAndHumidity(&actual_temperature, &actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_ReadMeasurementBuffer  */

void SHT3X_ReadMeasurementBuffer_get_SEND_COMMAND_FAIL_error_if_hardware_fails(
    void) {
    float temperature, humidity;
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode =
        sht3x_readMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_ReadMeasurementBuffer_get_SEND_COMMAND_FAIL_error_if_ACK_missing(
    void) {
    float temperature, humidity;
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode =
        sht3x_readMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_ReadMeasurementBuffer_get_RECEIVE_DATA_FAIL_error_if_hardware_fails(
    void) {
    float temperature, humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_Hardware_defect;

    sht3x_errorCode errorCode =
        sht3x_readMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_ReadMeasurementBuffer_get_RECEIVE_DATA_FAIL_error_if_ACK_missing(
    void) {
    float temperature, humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_ACK_missing;

    sht3x_errorCode errorCode =
        sht3x_readMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_RECEIVE_DATA_ERROR, errorCode);
}

void SHT3X_ReadMeasurementBuffer_get_CHECKSUM_FAIL_error(void) {
    float temperature, humidity;
    I2C_ReadCommand_ptr = I2C_ReadCommand_provoke_checksum_fail_for_SHT3X;

    sht3x_errorCode errorCode =
        sht3x_readMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_CHECKSUM_ERROR, errorCode);
}

void SHT3X_ReadMeasurementBuffer_read_successful(void) {
    float temperature, humidity;

    sht3x_errorCode errorCode =
        sht3x_readMeasurementBuffer(&temperature, &humidity);
    TEST_ASSERT_EQUAL_UINT8(SHT3X_NO_ERROR, errorCode);
}

void SHT3X_ReadMeasurementBuffer_read_correct_value(void) {
    float expected_temperature, actual_temperature, expected_humidity,
        actual_humidity;

    /* fill expected with random generated */
    uint16_t expected_rawValue_humidity = (byteZero << 8) | byteOne;
    expected_humidity =
        100.0f * ((float)expected_rawValue_humidity / (65536.0f - 1));

    uint16_t expected_rawValue_temperature = (byteZero << 8) | byteOne;
    expected_temperature =
        175.0f * ((float)expected_rawValue_temperature / (65536.0f - 1)) -
        45.0f;

    sht3x_readMeasurementBuffer(&actual_temperature, &actual_humidity);
    TEST_ASSERT_EQUAL_FLOAT(expected_temperature, actual_temperature);
    TEST_ASSERT_EQUAL_FLOAT(expected_humidity, actual_humidity);
}

/* endregion */
/* region SHT3X_EnableHeater  */

void SHT3X_EnableHeater_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_enableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_EnableHeater_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_enableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

/* endregion */
/* region SHT3X_DisableHeater  */

void SHT3X_DisableHeater_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_disableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_DisableHeater_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_disableHeater();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

/* endregion */
/* region SHT3X_SoftReset  */

void SHT3X_SoftReset_get_SEND_COMMAND_FAIL_error_if_hardware_fails(void) {
    I2C_WriteCommand_ptr = I2C_WriteCommand_Hardware_defect;

    sht3x_errorCode errorCode = sht3x_softReset();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

void SHT3X_SoftReset_get_SEND_COMMAND_FAIL_error_if_ACK_missing(void) {
    I2C_WriteCommand_ptr = I2C_WriteCommand_ACK_missing;

    sht3x_errorCode errorCode = sht3x_softReset();
    TEST_ASSERT_EQUAL_UINT8(SHT3X_SEND_COMMAND_ERROR, errorCode);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(
        SHT3X_ReadStatusRegister_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(
        SHT3X_ReadStatusRegister_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(
        SHT3X_ReadStatusRegister_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(
        SHT3X_ReadStatusRegister_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_ReadStatusRegister_get_CHECKSUM_FAIL_error);
    RUN_TEST(SHT3X_ReadStatusRegister_read_successful);
    RUN_TEST(SHT3X_ReadStatusRegister_read_correct_value);

    RUN_TEST(
        SHT3X_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_ReadSerialNumber_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(
        SHT3X_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_ReadSerialNumber_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_ReadSerialNumber_get_CHECKSUM_FAIL_error);
    RUN_TEST(SHT3X_ReadSerialNumber_read_successful);
    RUN_TEST(SHT3X_ReadSerialNumber_read_correct_value);

    RUN_TEST(
        SHT3X_GetTemperature_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_GetTemperature_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(
        SHT3X_GetTemperature_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_GetTemperature_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_GetTemperature_get_CHECKSUM_FAIL_error);
    RUN_TEST(SHT3X_GetTemperature_read_successful);
    RUN_TEST(SHT3X_GetTemperature_read_correct_value);

    RUN_TEST(SHT3X_GetHumidity_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_GetHumidity_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_GetHumidity_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_GetHumidity_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_GetHumidity_get_CHECKSUM_FAIL_error);
    RUN_TEST(SHT3X_GetHumidity_read_successful);
    RUN_TEST(SHT3X_GetHumidity_read_correct_value);

    RUN_TEST(
        SHT3X_GetTemperatureAndHumidity_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(
        SHT3X_GetTemperatureAndHumidity_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(
        SHT3X_GetTemperatureAndHumidity_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(
        SHT3X_GetTemperatureAndHumidity_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_GetTemperatureAndHumidity_get_CHECKSUM_FAIL_error);
    RUN_TEST(SHT3X_GetTemperatureAndHumidity_read_successful);
    RUN_TEST(SHT3X_GetTemperatureAndHumidity_read_correct_value);

    RUN_TEST(
        SHT3X_ReadMeasurementBuffer_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(
        SHT3X_ReadMeasurementBuffer_get_SEND_COMMAND_FAIL_error_if_ACK_missing);
    RUN_TEST(
        SHT3X_ReadMeasurementBuffer_get_RECEIVE_DATA_FAIL_error_if_hardware_fails);
    RUN_TEST(
        SHT3X_ReadMeasurementBuffer_get_RECEIVE_DATA_FAIL_error_if_ACK_missing);
    RUN_TEST(SHT3X_ReadMeasurementBuffer_get_CHECKSUM_FAIL_error);
    RUN_TEST(SHT3X_ReadMeasurementBuffer_read_successful);
    RUN_TEST(SHT3X_ReadMeasurementBuffer_read_correct_value);

    RUN_TEST(SHT3X_EnableHeater_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_EnableHeater_get_SEND_COMMAND_FAIL_error_if_ACK_missing);

    RUN_TEST(SHT3X_DisableHeater_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_DisableHeater_get_SEND_COMMAND_FAIL_error_if_ACK_missing);

    RUN_TEST(SHT3X_SoftReset_get_SEND_COMMAND_FAIL_error_if_hardware_fails);
    RUN_TEST(SHT3X_SoftReset_get_SEND_COMMAND_FAIL_error_if_ACK_missing);

    return UNITY_END();
}
