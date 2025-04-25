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

#define paramTest(fn, param)                                                                       \
    void fn##param(void) {                                                                         \
        fn(param);                                                                                 \
    }

/* region HELPER */

static uint16_t samplesForTrigger = 30;

/* endregion HELPER*/

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

void checkInitValues(adxl345bRegister_t registerToCheck) {
    uint8_t buffer;
    uint8_t expectedValue;
    if (registerToCheck == ADXL345B_REGISTER_BW_RATE) {
        expectedValue = ADXL345B_BW_RATE_LPM_12_point_5;
    } else if (registerToCheck == ADXL345B_REGISTER_POWER_CONTROL) {
        expectedValue = 0b00001000;
    } else if (registerToCheck == ADXL345B_REGISTER_INTERRUPT_ENABLE) {
        expectedValue = 0b00000000;
    } else if (registerToCheck == ADXL345B_REGISTER_DATA_FORMAT) {
        expectedValue = 0b00001000;
    } else {
        TEST_FAIL_MESSAGE("This register is not changed by init");
    }
    adxl345bErrorCode_t errorCode =
        adxl345bInternalReadDataFromSensor(sensor, registerToCheck, buffer, 1);
    if (errorCode == ADXL345B_NO_ERROR) {
        TEST_ASSERT_EQUAL(expectedValue, buffer);
    } else {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
}
paramTest(checkInitValues, ADXL345B_REGISTER_BW_RATE);
paramTest(checkInitValues, ADXL345B_REGISTER_POWER_CONTROL);
paramTest(checkInitValues, ADXL345B_REGISTER_INTERRUPT_ENABLE);
paramTest(checkInitValues, ADXL345B_REGISTER_DATA_FORMAT);


static void checkSerialNumber() {
    uint8_t serialNumber = 0;
    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(sensor, &serialNumber);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    TEST_ASSERT_EQUAL(0xE5, serialNumber);
}

static void makeSelfTest() {
    int delta_x, delta_y, delta_z;
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfTest(sensor, &delta_x, &delta_y, &delta_z);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
}

static void runCalibration() {
    PRINT("Start Calibration of ADXL345b");
    adxl345bErrorCode_t errorCode = adxl345bRunSelfCalibration(sensor);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("  \033[0;32mSUCCESSFUL\033[0m");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
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
    adxl345bErrorCode_t errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT, previousRange, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }

    if (previousRange != testedRange) {
        errorCode = adxl345bChangeMeasurementRange(sensor, testedRange);
        if (errorCode != ADXL345B_NO_ERROR) {
            adxl345bChangeMeasurementRange(sensor, previousRange);
            TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
        }
    }
    uint8_t buffer;
    errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_REGISTER_DATA_FORMAT, buffer, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        adxl345bChangeMeasurementRange(sensor, previousRange);
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    TEST_ASSERT_EQUAL(testedRange, buffer);
    if (previousRange != testedRange) {
        adxl345bChangeMeasurementRange(sensor, previousRange)
    };
}

paramTest(checkRangeValues, 2)
paramTest(checkRangeValues, 4)
paramTest(checkRangeValues, 8)
paramTest(checkRangeValues, 16)


/*! causes side effects: this function changes the FIFO-Mode persistently */
    void setGivenFifoModeAndCheckValues(uint8_t testedfifoMode) {
    adxl345bErrorCode_t adxl345bSetFIFOMode(sensor, testedFifoMode, samplesForTrigger);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    uint8_t buffer;
    errorCode = adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, buffer, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    uint8_t expectedFIFOConfiguration =
        (testedFifoMode & 0b11000000) | (samplesForTrigger & 0b00011111);
    TEST_ASSERT_EQUAL(expectedFIFOConfiguration, buffer);
}
paramTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_BYPASS);
paramTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_FIFO);
paramTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_STREAM);
paramTest(setGivenFifoModeAndCheckValues, ADXL345B_FIFOMODE_TRIGGER);

void checkSingleGValue(uint8_t testedFifoMode) {
    /* check current Fifo-Mode */
    uint8_t previousFifoMode;
    adxl345bErrorCode_t errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, previousFifoMode, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    /* set Fifo-Mode if necessary*/
    if (previousFifoMode != testedFifoMode) {
        errorCode = adxl345bSetFIFOMode(sensor, testedFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            adxl345bSetFIFOMode(sensor, previousFifoMode, samplesForTrigger);
            TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
        }
    }
    /* initialize Variables*/
    float xAxis = 0, yAxis = 0, zAxis = 0;
    uint8_t rawData[sizeOfRawData];
    adxl345bErrorCode_t errorCode;

    if (testedFifoMode != ADXL345B_FIFOMODE_BYPASS) {
        errorCode = adxl345bGetMultipleMeasurements(sensor, rawData, sizeOfRawData);
    } else {
        errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    }
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    errorCode = adxl345bConvertDataXYZ(&xAxis, &yAxis, &zAxis, rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
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
        adxl345bSetFIFOMode(sensor, prevousFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
        }
    }
}
paramTest(checkSingleGValue, ADXL345B_FIFOMODE_BYPASS);
paramTest(checkSingleGValue, ADXL345B_FIFOMODE_FIFO);
paramTest(checkSingleGValue, ADXL345B_FIFOMODE_STREAM);
paramTest(checkSingleGValue, ADXL345B_FIFOMODE_TRIGGER);

void checkMultipleGValues(uint8_t testedFifoMode) {
    /* check current Fifo-Mode */
    uint8_t previousifoMode;
    adxl345bErrorCode_t errorCode =
        adxl345bInternalReadDataFromSensor(sensor, ADXL345B_FIFO_CONTROL, previousFifoMode, 1);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }

    /* set Fifo-Mode if necessary*/
    if (previousFifoMode != testedFifoMode) {
        adxl345bSetFIFOMode(sensor, prevousFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
        }
    }

    /* initialize variables */
    float xAxis = 0, yAxis = 0, zAxis = 0;
    sizeOfRawData = 100;
    uint8_t rawData[sizeOfRawData];
    memset(rawData, 0, sizeOfRawData);
    adxl345bErrorCode_t errorCode;

    if (testedFifoMode == ADXL345B_FIFOMODE_BYPASS) {
        errorCode = adxl345bGetMultipleMeasurements(sensor, rawData, sizeOfRawData);
        if (errorCode != ADXL345B_NO_ERROR) {
            TEST_FAIL_MESSAGE(
                "calling getMultipleMeasurements in BYPASS-Mode should fail but does not");
        } else {
            TEST_FAIL_MESSAGE("calling getMultipleMeasurements in BYPASS-Mode should fail");
        }
    } else {
        errorCode = adxl345bGetSingleMeasurement(sensor, rawData);
    }
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    errorCode = adxl345bConvertDataXYZ(&xAxis, &yAxis, &zAxis, rawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
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
        adxl345bSetFIFOMode(sensor, prevousFifoMode, samplesForTrigger);
        if (errorCode != ADXL345B_NO_ERROR) {
            TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
        }
    }
}

paramTest(checkMultipleGValues, ADXL345B_FIFOMODE_BYPASS);
paramTest(checkMultipleGValues, ADXL345B_FIFOMODE_FIFO);
paramTest(checkMultipleGValues, ADXL345B_FIFOMODE_STREAM);
paramTest(checkMultipleGValues, ADXL345B_FIFOMODE_TRIGGER);

void checkGValuesWithMeasuringForNMilliseconds(uint32_t milliseconds) {
    uint8_t numberOfSamples = 103;

    /*generate Array for Data (which is not modulo6 == 0)*/
    uint8_t wrongSize = 2;
    uint32_t sizeOfRequestedRawData = numberOfSamples * sizeOfRawData + wrongSize;
    uint8_t samples[sizeOfRequestedRawData];
    memset(samples, 0, sizeOfRequestedRawData);

    /*generate Array for GValues of X-,Y- and Z-Axis*/
    uint8_t numberOfGValuesPerSample = 3;
    uint32_t sizeOfGValues = numberOfSamples * numberOfGValuesPerSample;
    float gValues[sizeOfGValues];
    memset(gValues, 0, sizeOfGValues);

    /*generate Array for sums of Axis*/
    float sumsOfAxis[numberOfSamples];
    memset(sumsOfAxis, 0, numberOfSamples);

    adxl345bErrorCode_t errorCode;

    errorCode = adxl345bGetMeasurementsForNMilliseconds(sensor, samples, milliseconds,
                                                        &sizeOfRequestedRawData);
    if (errorCode != ADXL345B_NO_ERROR) {
        TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
    }
    for (int i = 0; i < sizeOfRequestedRawData; i += sizeOfRawData) {
        errorCode = adxl345bConvertDataXYZ(&(gValues + i), &(gValues + i + 2), &(gValues + i + 4),
                                           samples + i);
        if (errorCode != ADXL345B_NO_ERROR) {
            TEST_FAIL_MESSAGE("ADXL_ERROR occurred");
        }
    }

    for (int i = 0; i < sizeOfGValues; i += numberOfGValuesPerSample) {
        /* 0.2G equals a deviation of about 1% from the ideal value
         * this deviation is given by the datasheet as the accepted tolerance
         * for each axis therefore should epsilon be 0.6G
         */
        sumsOfAxis =
            floatToAbs(gValues + i) + floatToAbs(gValues + i + 1) + floatToAbs(gValues + i + 2);
    }
    // this could maybe fail. Better test range
    TEST_ASSERT_EACH_EQUAL_FLOAT(1.0f, sumsOfAxis);

    TEST_ASSERT_EACH_FLOAT_WITHIN(1.0f, sumsOfAxis, 0.6f);
}
/*should be tested with all FIFO-Modes*/
paramTest(checkGValuesWithMeasuringForNMilliseconds, 1);
paramTest(checkGValuesWithMeasuringForNMilliseconds, 2);
paramTest(checkGValuesWithMeasuringForNMilliseconds, 3);
paramTest(checkGValuesWithMeasuringForNMilliseconds, 10);
paramTest(checkGValuesWithMeasuringForNMilliseconds, 1000);
/*! collects one minute rawData for GValues: */
paramTest(checkGValuesWithMeasuringForNMilliseconds, 60000);

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
    RUN_TEST(makeSelfTest);
    RUN_TEST(runCalibration);

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
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds60000);
    /* endregion TESTS IN BYPASS-MODE */

    /* region TESTS IN FIFO-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_FIFO);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_FIFO);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_FIFO);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds60000);
    /* endregion TESTS IN FIFO-MODE */

    /* region TESTS IN STREAM-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_STREAM);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_STREAM);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_STREAM);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds60000);
    /* endregion TESTS IN STREAM-MODE */

    /* region TESTS IN TRIGGER-MODE */
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_TRIGGER);
    RUN_TEST(checkSingleGValueADXL345B_FIFOMODE_TRIGGER);
    RUN_TEST(checkMultipleGValuesADXL345B_FIFOMODE_TRIGGER);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds2);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds3);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds10);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds1000);
    RUN_TEST(checkGValuesWithMeasuringForNMilliseconds60000);
    /* endregion TESTS IN TRIGGER-MODE */

    UNITY_END();
    deInit();

    return 0;
}
