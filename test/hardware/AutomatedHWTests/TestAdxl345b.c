#define SOURCE_FILE "ADXL345B-HWTEST"

#include "eai/Common.h"
#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/I2c.h"
#include "eai/sensor/Adxl345b.h"

#include "hardware/i2c.h"
#include "unity.h"
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>

#define parameterTest(fn, parameter)                                                               \
    void fn##parameter(void) {                                                                     \
        fn(parameter);                                                                             \
    }

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = I2C_MODULE,
    .frequency = I2C_FREQUENCY_IN_HZ,
    .sdaPin = I2C_SDA_PIN,
    .sclPin = I2C_SCL_PIN,
};

/* endregion I2C DEFINITION */

/* region SENSOR DEFINITION */
adxl345bSensorConfiguration_t sensor = {
    .i2c_slave_address = ADXL_SLAVE,
    .i2c_host = ADXL_I2C_MODULE,
};
/* endregion SENSOR DEFINITION */

/* region HELPER */
static uint16_t samplesForTrigger = 30;
uint16_t sizeOfRawData = 20;

static adxl345bErrorCode_t adxl345bInternalReadDataFromSensor(adxl345bSensorConfiguration_t sensor,
                                                              adxl345bRegister_t registerToRead,
                                                              uint8_t *responseBuffer,
                                                              uint8_t sizeOfResponseBuffer) {
    uint8_t sizeOfCommandBuffer = 1;
    uint8_t commandBuffer[sizeOfCommandBuffer];
    commandBuffer[0] = registerToRead;

    PRINT_DEBUG("requesting data from sensor");
    i2cErrorCode_t i2cErrorCode = i2cWriteCommand(sensor.i2c_host, sensor.i2c_slave_address,
                                                  commandBuffer, sizeOfCommandBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("sending request failed");
        return ADXL345B_SEND_COMMAND_ERROR;
    }

    PRINT_DEBUG("receiving data from sensor");
    i2cErrorCode = i2cReadData(sensor.i2c_host, sensor.i2c_slave_address, responseBuffer,
                               sizeOfResponseBuffer);
    if (i2cErrorCode != I2C_NO_ERROR) {
        PRINT_DEBUG("receiving data failed");
        return ADXL345B_RECEIVE_DATA_ERROR;
    }

    PRINT_DEBUG("successful received data from sensor");
    return ADXL345B_NO_ERROR;
}

static float floatToAbs(float input) {
    if (input < 0) {
        return (-1) * input;
    } else {
        return input;
    }
}

static adxl345bErrorCode_t changeTestedFifoMode(uint8_t *previousFifoMode, uint8_t testedFifoMode) {
    /* check current Fifo-Mode */
    adxl345bErrorCode_t errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, previousFifoMode, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("Internal Reading failed; adxl345b_ERROR: %02X", errorCode);
        return errorCode;
    }
    /* set Fifo-Mode if necessary*/
    if (*previousFifoMode != testedFifoMode) {
        errorCode = adxl345bSetFIFOMode(sensor, testedFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            adxl345bSetFIFOMode(sensor, *previousFifoMode, samplesForTrigger);
            PRINT("Setting FifoMode failed; adxl345b_ERROR: %02X", errorCode);
        }
    }
    return errorCode;
}

/* endregion HELPER */

void init() {
    /* enable print to console */
    stdio_init_all();
    /* wait for user console to connect */
    while (!stdio_usb_connected()) {}
    sleep_ms(500);

    /* initialize I2C */
    i2cErrorCode_t i2cErrorCode;
    while (1) {
        i2cErrorCode = i2cInit(&i2cConfig);
        if (i2cErrorCode == I2C_NO_ERROR) {
            break;
        }
        PRINT("Initialise I2C failed; i2c_ERROR: %02X", i2cErrorCode);
        sleep_ms(500);
    }

    /* initialize ADXL345B sensor */
    adxl345bErrorCode_t errorCode;
    while (1) {
        errorCode = adxl345bInit(sensor);
        if (errorCode == ADXL345B_NO_ERROR) {
            break;
        }
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
        sleep_ms(500);
    }
}

static void checkInitValues(adxl345bRegister_t registerToCheck) {
    uint8_t buffer;
    uint8_t expectedValue;
    if (registerToCheck == ADXL345B_REGISTER_BW_RATE) {
        expectedValue = ADXL345B_BW_RATE_LPM_12_point_5;
    } else if (registerToCheck == ADXL345B_REGISTER_POWER_CONTROL) {
        expectedValue = 0b00001000;
    } else if (registerToCheck == ADXL345B_REGISTER_INTERRUPT_ENABLE) {
        expectedValue = 0b00000000;
    } else if (registerToCheck == ADXL345B_REGISTER_DATA_FORMAT) {
        expectedValue = 0b00000000;
    } else {
        TEST_FAIL_MESSAGE("This register is not changed by init");
    }
    adxl345bErrorCode_t errorCode =
        adxl345bInternalReadDataFromSensor(sensor, registerToCheck, &buffer, 1);
    if (errorCode == ADXL345B_NO_ERROR) {
        TEST_ASSERT_EQUAL(expectedValue, buffer);
    } else {
        PRINT("checkInitValues failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
}
parameterTest(checkInitValues, ADXL345B_REGISTER_BW_RATE);
parameterTest(checkInitValues, ADXL345B_REGISTER_POWER_CONTROL);
parameterTest(checkInitValues, ADXL345B_REGISTER_INTERRUPT_ENABLE);
parameterTest(checkInitValues, ADXL345B_REGISTER_DATA_FORMAT);

static void checkSerialNumber() {
    uint8_t serialNumber = 0;
    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bReadSerialNumber failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    TEST_ASSERT_EQUAL_HEX8(0xE5, serialNumber);
}

static void makeSelfTest() {
    int delta_x, delta_y, delta_z;
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfTest(sensor, &delta_x, &delta_y, &delta_z);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bPerformSelfTest failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
}

static void runCalibration() {
    adxl345bErrorCode_t errorCode = adxl345bRunSelfCalibration(sensor);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("adxl345bRunSelfCalibration successful; adxl345b_ERROR: %02X", errorCode);
        TEST_PASS();
    } else {
        PRINT("adxl345bRunSelfCalibration failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
}


void checkRangeValues(uint8_t testedRange) {
    if (testedRange == 2) {
        testedRange = (adxl345bRange_t)0b00001000;
    } else if (testedRange == 4) {
        testedRange = (adxl345bRange_t)0b00001001;
    } else if (testedRange == 8) {
        testedRange = (adxl345bRange_t)0b00001010;
    } else if (testedRange == 16) {
        testedRange = (adxl345bRange_t)0b00001011;
    } else {
        TEST_FAIL_MESSAGE("This range does not exist");
    }

    uint8_t previousRange;
    adxl345bErrorCode_t errorCode = adxl345bInternalReadDataFromSensor(
        sensor, ADXL345B_REGISTER_DATA_FORMAT, &previousRange, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("InternalReadDataFromSensor failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

    if (previousRange != testedRange) {
        errorCode = adxl345bChangeMeasurementRange(sensor, testedRange);
        if (errorCode != ADXL345B_NO_ERROR) {
            adxl345bChangeMeasurementRange(sensor, previousRange);
            PRINT("adxl345bChangeMeasurementRange failed; adxl345b_ERROR: %02X", errorCode);
            TEST_FAIL();
        }
    }
    uint8_t buffer;
    errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT, &buffer, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        adxl345bChangeMeasurementRange(sensor, previousRange);
        PRINT("adxl345bChangeMeasurementRange failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    TEST_ASSERT_EQUAL(testedRange, buffer);
    if (previousRange != testedRange) {
        adxl345bChangeMeasurementRange(sensor, previousRange);
    }
}

parameterTest(checkRangeValues, 2) parameterTest(checkRangeValues, 4)
    parameterTest(checkRangeValues, 8) parameterTest(checkRangeValues, 16)

    /*! causes side effects: this function changes the FIFO-Mode persistently */
    void setGivenFifoModeAndCheckValues(uint8_t testedFifoMode) {
    adxl345bErrorCode_t errorCode = adxl345bSetFIFOMode(sensor, testedFifoMode, samplesForTrigger);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bSetFIFOMode failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    uint8_t buffer;
    errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, &buffer, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bInternalReadDataFromSensor failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    uint8_t expectedFIFOConfiguration =
        (testedFifoMode & 0b11000000) | (samplesForTrigger & 0b00011111);
    TEST_ASSERT_EQUAL(expectedFIFOConfiguration, buffer);
}
parameterTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_BYPASS);
parameterTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_FIFO);
parameterTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_STREAM);
parameterTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_TRIGGER);

void checkSingleGValue(uint8_t testedFifoMode) {
    uint8_t previousFifoMode;
    adxl345bErrorCode_t errorCode = changeTestedFifoMode(&previousFifoMode, testedFifoMode);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("changeTestedFifoMode failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    /* initialize Variables*/
    float xAxis = 0, yAxis = 0, zAxis = 0;
    adxl345bRawData_t *rawData;

    if (testedFifoMode != ADXL345B_FIFOMODE_BYPASS) {
        errorCode = adxl345bGetMultipleMeasurements(sensor, rawData, 1);
    } else {
        errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    }
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bGet[...]Measurement failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    errorCode = adxl345bConvertDataXYZ(sensor, &xAxis, &yAxis, &zAxis, *rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bConvertDataXYZ failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

    /* 0.2G equals a deviation of about 1% from the ideal value
     * this deviation is given by the datasheet as the accepted tolerance
     * for each axis therefore should epsilon be 0.6G
     */
    float sumOfAxis = floatToAbs(xAxis) + floatToAbs(yAxis) + floatToAbs(zAxis);

    // TODO: this could maybe fail. Better test range
    TEST_ASSERT_EQUAL_FLOAT(1.0f, sumOfAxis);

    TEST_ASSERT_FLOAT_WITHIN(1.0f, sumOfAxis, 0.6f);

    /* reset Fifo-Mode if necessary */
    if (previousFifoMode != testedFifoMode) {
        adxl345bSetFIFOMode(sensor, previousFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT("adxl345bSetFIFOMode failed; adxl345b_ERROR: %02X", errorCode);
            TEST_FAIL();
        }
    }
}
parameterTest(checkSingleGValue, ADXL345B_FIFOMODE_BYPASS);
parameterTest(checkSingleGValue, ADXL345B_FIFOMODE_FIFO);
parameterTest(checkSingleGValue, ADXL345B_FIFOMODE_STREAM);
parameterTest(checkSingleGValue, ADXL345B_FIFOMODE_TRIGGER);

void checkMultipleGValues(uint8_t testedFifoMode) {
    uint8_t previousFifoMode;
    adxl345bErrorCode_t errorCode = changeTestedFifoMode(&previousFifoMode, testedFifoMode);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("changeTestedFifoMode failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }

    /* initialize variables */
    float xAxis = 0, yAxis = 0, zAxis = 0;
    adxl345bRawData_t *rawData;

    if (testedFifoMode == ADXL345B_FIFOMODE_BYPASS) {
        errorCode = adxl345bGetMultipleMeasurements(sensor, rawData, sizeOfRawData);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT( "calling adxl345bGetMultipleMeasurements in BYPASS-Mode should fail but does not; adxl345b_ERROR: %02X", errorCode);
            TEST_FAIL();
        } else {
            PRINT("calling adxl345GgetMultipleMeasurements in BYPASS-Mode should fail; adxl345b_ERROR: %02X", errorCode);
            TEST_FAIL();
        }
    } else {
        errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    }
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bGetSingleMeasurement failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    errorCode = adxl345bConvertDataXYZ(sensor, &xAxis, &yAxis, &zAxis, *rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bConvertDataXYZ failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    /* 0.2G equals a deviation of about 1% from the ideal value
     * this deviation is given by the datasheet as the accepted tolerance
     * for each axis therefore should epsilon be 0.6G
     */
    float sumOfAxis = floatToAbs(xAxis) + floatToAbs(yAxis) + floatToAbs(zAxis);

    TEST_ASSERT_FLOAT_WITHIN(1.0f, sumOfAxis, 0.6f);

    /* reset Fifo-Mode if necessary */
    if (previousFifoMode != testedFifoMode) {
        adxl345bSetFIFOMode(sensor, previousFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT("adxl345bSetFIFOMode failed; adxl345b_ERROR: %02X", errorCode);
            TEST_FAIL();
        }
    }
}

parameterTest(checkMultipleGValues, ADXL345B_FIFOMODE_BYPASS);
parameterTest(checkMultipleGValues, ADXL345B_FIFOMODE_FIFO);
parameterTest(checkMultipleGValues, ADXL345B_FIFOMODE_STREAM);
parameterTest(checkMultipleGValues, ADXL345B_FIFOMODE_TRIGGER);

void checkGValuesWithMeasuringForNMicroseconds(uint32_t microseconds) {
    uint8_t numberOfSamples = 103;

    adxl345bRawData_t rawData[numberOfSamples];

    /*generate Array for GValues of X-,Y- and Z-Axis*/
    uint8_t numberOfGValuesPerSample = 3;
    uint32_t sizeOfGValues = numberOfSamples * numberOfGValuesPerSample;
    float gValues[sizeOfGValues];
    memset(gValues, 0, sizeOfGValues);

    /*generate Array for sums of Axis*/
    float sumsOfAxis[numberOfSamples];
    memset(sumsOfAxis, 0, numberOfSamples);

    adxl345bErrorCode_t errorCode;

    errorCode = adxl345bGetMeasurementsForNMicroseconds(sensor, microseconds, rawData,
                                                        numberOfSamples);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("adxl345bGetMeasurementsForNMicroseconds failed; adxl345b_ERROR: %02X", errorCode);
        TEST_FAIL();
    }
    for (int i = 0; i < numberOfSamples * numberOfGValuesPerSample; i+= numberOfGValuesPerSample) {
        errorCode = adxl345bConvertDataXYZ(sensor, (gValues + i), (gValues + i+1), (gValues + i+2),
                                           *(rawData+i));
        if (errorCode != ADXL345B_NO_ERROR) {
            PRINT("adxl345bConvertDataXYZ failed; adxl345b_ERROR: %02X", errorCode);
            TEST_FAIL();
        }
    }
    int j = 0;
    for (int i = 0; i < sizeOfGValues; i += numberOfGValuesPerSample) {
        /* 0.2G equals a deviation of about 1% from the ideal value
         * this deviation is given by the datasheet as the accepted tolerance
         * for each axis therefore should epsilon be 0.6G
         */
        sumsOfAxis[j] = floatToAbs(*(gValues+i)) + floatToAbs(*(gValues+i+1)) + floatToAbs(*(gValues+i+2));
        j++;
        //TEST_ASSERT_FLOAT_WITHIN(1.0f, sumsOfAxis, 0.6f);
    }

        TEST_ASSERT_EACH_EQUAL_FLOAT(0.0f, sumsOfAxis, numberOfSamples);
}
/*should be tested with all FIFO-Modes*/
parameterTest(checkGValuesWithMeasuringForNMicroseconds, 1);
parameterTest(checkGValuesWithMeasuringForNMicroseconds, 2);
parameterTest(checkGValuesWithMeasuringForNMicroseconds, 3);
parameterTest(checkGValuesWithMeasuringForNMicroseconds, 10);
parameterTest(checkGValuesWithMeasuringForNMicroseconds, 1000);
/*! collects one minute rawData for GValues: */
parameterTest(checkGValuesWithMeasuringForNMicroseconds, 60000);

void setUp() {}
void tearDown() {};

void deInit() {
    rom_reset_usb_boot(0, 0);
}

int main() {
    init();
    UNITY_BEGIN();
    /* region checkInitValues */
    RUN_TEST(checkInitValuesADXL345B_REGISTER_BW_RATE);
    RUN_TEST(checkInitValuesADXL345B_REGISTER_POWER_CONTROL);
    RUN_TEST(checkInitValuesADXL345B_REGISTER_INTERRUPT_ENABLE);
    RUN_TEST(checkInitValuesADXL345B_REGISTER_DATA_FORMAT);
    /* endregion checkInitValues */

    RUN_TEST(checkSerialNumber);
    //RUN_TEST(makeSelfTest);
    //RUN_TEST(runCalibration);

    /* region checkRangeValues */
    RUN_TEST(checkRangeValues2);
    RUN_TEST(checkRangeValues4);
    RUN_TEST(checkRangeValues8);
    RUN_TEST(checkRangeValues16);
    /* endregion checkRangeValues */

    /* region TESTS IN BYPASS-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_BYPASS);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_BYPASS);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_BYPASS);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds60000);
    /* endregion TESTS IN BYPASS-MODE */

    /* region TESTS IN FIFO-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_FIFO);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_FIFO);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_FIFO);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds60000);
    /* endregion TESTS IN FIFO-MODE */

    /* region TESTS IN STREAM-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_STREAM);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_STREAM);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_STREAM);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds60000);
    /* endregion TESTS IN STREAM-MODE */

    /* region TESTS IN TRIGGER-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_TRIGGER);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_TRIGGER);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_TRIGGER);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMicroseconds60000);
    /* endregion TESTS IN TRIGGER-MODE */

    UNITY_END();
    deInit();

    return 0;
}