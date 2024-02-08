#define SOURCE_FILE "ADXL345-Test"

#include "Adxl345b.h"
#include "Common.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>

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

/* endregion */

static void getSerialNumber() {
    uint8_t serialNumber = 0;

    PRINT("Requesting serial number.");
    adxl345bErrorCode_t errorCode = adxl345bReadSerialNumber(&serialNumber);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("  Expected: 0xE5, Actual: 0x%02X", serialNumber);
        PRINT(serialNumber == 0xE5 ? "  \033[0;32mPASSED\033[0m" : "  \033[0;31mFAILED\033[0m;");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m adxl345b_ERROR: %02X", errorCode);
    }
}

static void getGValue() {
    float xAxis = 0, yAxis = 0, zAxis = 0;

    PRINT("Requesting g values.");
    adxl345bErrorCode_t errorCode = adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);
    if (errorCode == ADXL345B_NO_ERROR) {
        /* 0.2G equals a deviation of about 1% from the ideal value
         * this deviation is given by the datasheet as the accepted tolerance
         * for each axis therefore should epsilon be 0.6G
         */
        float sumOfAxis = floatToAbs(xAxis) + floatToAbs(yAxis) + floatToAbs(zAxis);

        PRINT("  Expected: 01.0000G, Actual: %2.4fG = |%2.4fG| + |%2.4fG| + "
              "|%2.4fG| = X + Y + Z",
              sumOfAxis, xAxis, yAxis, zAxis);
        PRINT(compareFloatsWithinRange(1.0f, sumOfAxis, 0.6f) ? "  \033[0;32mPASSED\033[0m"
                                                              : "  \033[0;31mFAILED\033[0m");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
    }
}

static void makeSelfTest() {
    PRINT("Start self test:");
    int delta_x, delta_y, delta_z;
    adxl345bErrorCode_t errorCode = adxl345bPerformSelfTest(&delta_x, &delta_y, &delta_z);
    PRINT("  X: %iLSB, Y: %iLSB, Z: %iLSB", delta_x, delta_y, delta_z);
    if (errorCode == ADXL345B_NO_ERROR) {
        PRINT("  \033[0;32mPASSED\033[0m");
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X", errorCode);
    }
}

static void runCalibration() {
    PRINT("Start Calibration:");
    adxl345bErrorCode_t errorCode = adxl345bRunSelfCalibration();
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

    /* initialize ADXL345B sensor */
    PRINT("START INIT");
    adxl345bErrorCode_t errorCode;
    while (1) {
        errorCode = adxl345bInit(i2c0, ADXL345B_I2C_ALTERNATE_ADDRESS);
        if (errorCode == ADXL345B_NO_ERROR) {
            PRINT("Initialised ADXL345B.");
            break;
        }
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode);
        sleep_ms(500);
    }

    /* test function of ADXL345B */
    PRINT("Please enter to request g (G value), s (serialNo), t (self test), "
          "c (calibration) or b (Boot mode)");
    while (1) {
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
        default:
            PRINT("Please enter to request g (G value), s (serialNo), t (self test), c "
                  "(calibration) or b (Boot mode)");
            break;
        }
    }

    return 0;
}
