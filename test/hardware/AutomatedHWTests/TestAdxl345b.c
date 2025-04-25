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

void setUp() {}
void tearDown() {};

void deInit() {
    rom_reset_usb_boot(0, 0);
}

int main() {
    init();
    UNITY_BEGIN();
    RUN_TEST(checkInitValuesADXL345B_REGISTER_BW_RATE);
    RUN_TEST(checkInitValuesADXL345B_REGISTER_POWER_CONTROL);
    RUN_TEST(checkInitValuesADXL345B_REGISTER_INTERRUPT_ENABLE);
    RUN_TEST(checkInitValuesADXL345B_REGISTER_DATA_FORMAT);
    UNITY_END();
    deInit();

    return 0;
}
