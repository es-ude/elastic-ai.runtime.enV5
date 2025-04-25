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

    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_BYPASS);
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_FIFO);
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_STREAM);
    RUN_TEST(setGivenFifoModeAndCheckValuesADXL345B_FIFOMODE_TRIGGER);

    UNITY_END();
    deInit();

    return 0;
}
