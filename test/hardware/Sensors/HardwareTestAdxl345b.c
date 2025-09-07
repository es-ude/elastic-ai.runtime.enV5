#define SOURCE_FILE "ADXL345-Test"

#include "hardware/i2c.h"
#include "pico/bootrom.h"
#include "pico/stdio_usb.h"

#include "eai/Common.h"
#include "eai/hal/I2c.h"
#include "eai/hal/Time.h"
#include "eai/sensor/Adxl345b.h"
#include "eai/sensor/Adxl345bTypedefs.h"

/* region HELPER */

float floatToAbs(float input) {
    if (input < 0) {
        return (-1) * input;
    } else {
        return input;
    }
}

_Bool compareFloatsWithinRange(float expected, float actual, float epsilon) {
    return floatToAbs(expected - actual) <= epsilon;
}

/* endregion HELPER*/

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = i2c1,
    .frequency = 400000,
    .sdaPin = 6,
    .sclPin = 7,
};
/* endregion I2C DEFINITION */

/* region SENSOR DEFINITION */
adxl345bSensorConfiguration_t sensor = {
    .i2c_slave_address = ADXL345B_I2C_ALTERNATE_ADDRESS,
    .i2c_host = i2c1,
};
/* endregion SENSOR DEFINITION */

static void getSerialNumber() {
    uint8_t serialNumber = 0;

    PRINT("Requesting serial number.");
    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(&sensor, &serialNumber);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("  Expected: 0xE5, Actual: 0x%02X", serialNumber);
        PRINT(serialNumber == 0xE5 ? "  \033[0;32mPASSED\033[0m" : "  \033[0;31mFAILED\033[0m;");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }
}

static void getGValue() {
    float xAxis = 0, yAxis = 0, zAxis = 0;

    adxl345bErrorCode_t errorCode;

    errorCode = adxl345bSetFullResolutionMode(&sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    errorCode = adxl345bWriteConfigurationToSensor(&sensor, ADXL345B_REGISTER_BW_RATE,
                                                   ADXL345B_BW_RATE_1_point_56);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    errorCode = adxl345bSetFIFOMode(&sensor, ADXL345B_FIFOMODE_STREAM, 0);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    sleep_ms(2);
    errorCode = adxl345bActivateMeasurementMode(&sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }

    sleep_ms(10); // wait for measurement mode to be active

    PRINT("Waiting for data ready interrupt.");
    do {
        errorCode = adxl345bCheckInterruptSet(&sensor, ADXL345B_INTERRUPT_DATA_READY);
        if (errorCode != ADXL345B_INTERRUP_NOT_SET && errorCode != ADXL345B_NO_ERROR) {
            PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
            return;
        } else if (errorCode == ADXL345B_NO_ERROR) {
            break;
        }
        sleep_ms(1);
    } while (true);

    PRINT("Requesting g values.");
    adxl345bRawData_t rawData;
    errorCode = adxl345bGetSingleMeasurement(&sensor, &rawData);
    if (errorCode == ADXL345B_NO_ERROR) {
        adxl345bLsbSample_t lsbSample;
        errorCode = adxl345bConvertBytesToLSB(rawData, &lsbSample);
        if (errorCode == ADXL345B_NO_ERROR) {
            adxl345bGValueSample_t gValues;
            errorCode = adxl345bConvertLSBToGValue(lsbSample, &gValues);
            if (errorCode == ADXL345B_NO_ERROR) {

                /* 0.2G equals a deviation of about 1% from the ideal value
                 * this deviation is given by the datasheet as the accepted tolerance
                 * for each axis therefore should epsilon be 0.6G
                 */
                float sumOfAxis = gValues.x + gValues.y + gValues.z;

                PRINT("  Expected: 01.0000G, Actual: %2.4fG = %2.4fG + %2.4fG + %2.4fG = X + Y "
                      "+ Z",
                      sumOfAxis, gValues.x, gValues.y, gValues.z);
                PRINT(compareFloatsWithinRange(1.0f, sumOfAxis, 0.6f)
                          ? "  \033[0;32mPASSED\033[0m"
                          : "  \033[0;31mFAILED\033[0m");
            } else {
                PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
            }
        } else {
            PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
        }
    }

    errorCode = adxl345bDeactivateMeasurementMode(&sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
}

void configureSensor() {
    adxl345bErrorCode_t errorCode;

    errorCode = adxl345bSetFullResolutionMode(&sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    errorCode = adxl345bChangeMeasurementRange(&sensor, ADXL345B_8G_RANGE);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    errorCode = adxl345bWriteConfigurationToSensor(&sensor, ADXL345B_REGISTER_BW_RATE,
                                                   ADXL345B_BW_RATE_1_point_56);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    errorCode = adxl345bSetFIFOMode(&sensor, ADXL345B_FIFOMODE_STREAM, 0);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    sleep_ms(10);
    PRINT("CONFIGURED");
}

void record_three_seconds() {
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bActivateMeasurementMode(&sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    sleep_ms(3000);
    errorCode = adxl345bDeactivateMeasurementMode(&sensor);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
        return;
    }
    PRINT("DONE RECORDING");
}

void read_measurement() {
    adxl345bRawData_t sample = {};
    adxl345bErrorCode_t errorCode;
    errorCode = adxl345bGetSingleMeasurement(&sensor, &sample);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("RAW: %012X", sample.rawData);
    } else {
        PRINT("\033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }

    adxl345bLsbSample_t lsbSample = {};
    errorCode = adxl345bConvertBytesToLSB(sample, &lsbSample);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("LSB: X=%i - Y=%i - Z=%i", lsbSample.x, lsbSample.y, lsbSample.z);
    } else {
        PRINT("\033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }

    adxl345bGValueSample_t gvalueSample = {};
    errorCode = adxl345bConvertLSBToGValue(lsbSample, &gvalueSample);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("G-Value: X=%f - Y=%f - Z=%f", gvalueSample.x, gvalueSample.y, gvalueSample.z);
    } else {
        PRINT("\033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }
}

static void getFifoStatus() {
    uint8_t fifoRegister = 0;
    adxl345bErrorCode_t errorCode =
        adxl345bReadConfigurationFromSensor(&sensor, ADXL345B_FIFO_STATUS, &fifoRegister);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("FIFO Status: 0x%02X", fifoRegister);
    } else {
        PRINT("\033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }
}

static void getIntStatus() {
    uint8_t intRegister = 0;

    adxl345bErrorCode_t errorCode =
        adxl345bReadConfigurationFromSensor(&sensor, ADXL345B_REGISTER_INT_SOURCE, &intRegister);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("Interrupt Status: 0x%02X", intRegister);
    } else {
        PRINT("\033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }
}
static void getBwStatus() {
    uint8_t intRegister = 0;

    adxl345bErrorCode_t errorCode =
        adxl345bReadConfigurationFromSensor(&sensor, ADXL345B_REGISTER_BW_RATE, &intRegister);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("BW Rate: 0x%02X", intRegister);
    } else {
        PRINT("\033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }
}
static void makeSelfTest() {
    PRINT("Start self test:");
    int delta_x, delta_y, delta_z;
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfTest(&sensor);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("  \033[0;32mPASSED\033[0m");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: 0x%02X", errorCode);
    }
}

static void runCalibration() {
    PRINT("Start Calibration:");
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfCalibration(&sensor);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("  \033[0;32mSUCCESSFUL\033[0m");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
    }
}

static void enterBootMode() {
    reset_usb_boot(0, 0);
}

int main(void) {
    /* enable print to console */
    stdio_init_all();
    // wait for user console to connect
    while ((!stdio_usb_connected())) {}
    sleep_ms(500);

    /* initialize I2C */
    PRINT("===== START I2C INIT =====");
    i2cErrorCode_t i2cErrorCode;
    while (1) {
        i2cErrorCode = i2cInit(&i2cConfig);
        if (i2cErrorCode == I2C_NO_ERROR) {
            PRINT("Initialised I2C.");
            break;
        }
        PRINT("Initialise I2C failed; i2c_ERROR: %02X", i2cErrorCode);
        sleep_ms(500);
    }

    /* initialize ADXL345B sensor */
    PRINT("===== START ADXL345B INIT =====");
    adxl345bErrorCode_t errorCode;
    while (1) {
        errorCode = adxl345bInit(&sensor);
        if (errorCode == ADXL345B_NO_ERROR) {
            PRINT("Initialised ADXL345B.");
            break;
        }
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
        sleep_ms(500);
    }

    /* test function of ADXL345B */
    PRINT("===== START TEST =====");
    PRINT("Please enter to request g (G value), s (serialNo), t (self test), "
          "c (calibration) or b (Boot mode)");

    while (1) {
        uint32_t loc = timeUs32();

        char input = getchar_timeout_us(10000000); /* 10 seconds wait */

        switch (input) {
        case 'g':
            getGValue();
            break;
        case 's':
            getSerialNumber();
            break;
        case 't':
            makeSelfTest();
            break;
        case 'c':
            runCalibration();
            break;
        case 'b':
            enterBootMode();
            break;
        case '1':
            getFifoStatus();
            break;
        case '2':
            getIntStatus();
            break;
        case '3':
            getBwStatus();
            break;
        case '4':
            configureSensor();
            break;
        case '5':
            record_three_seconds();
            break;
        case '6':
            read_measurement();
            break;
        default:
            PRINT("Please enter to request g (G value), s (serialNo), t (self test), c "
                  "(calibration) or b (Boot mode)");
            break;
        }
    }

    return 0;
}
