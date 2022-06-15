//
// Created by David P. Federl
//

#include "adxl345b_public.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>

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

    printf("Requesting serial number.\n");
    adxl345b_errorCode errorCode = adxl345b_readSerialNumber(&serialNumber);
    if (errorCode == ADXL345B_NO_ERROR) {
        printf(serialNumber == 0xE5 ? "  \033[0;32mPASSED\033[0m; "
                                    : "  \033[0;31mFAILED\033[0m; ");
        printf("Expected: 0xE5, Actual: 0x%02X\n", serialNumber);
    } else {
        printf("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X\n", errorCode);
    }
}

static void getGValue() {
    float xAxis = 0, yAxis = 0, zAxis = 0;

    printf("Requesting g values.\n");
    adxl345b_errorCode errorCode =
        adxl345b_readMeasurements(&xAxis, &yAxis, &zAxis);
    if (errorCode == ADXL345B_NO_ERROR) {
        /* 0.2G equals a deviation of about 1% from the ideal value
         * this deviation is given by the datasheet as the accepted tolerance
         * for each axis therefore should epsilon be 0.6G
         */
        float sumOfAxis =
            floatToAbs(xAxis) + floatToAbs(yAxis) + floatToAbs(zAxis);
        printf(compareFloatsWithinRange(1.0f, sumOfAxis, 0.6f)
                   ? "  \033[0;32mPASSED\033[0m; "
                   : "  \033[0;31mFAILED\033[0m; ");
        printf("Expected: 01.0000G, Actual: %2.4fG = |%2.4fG| + |%2.4fG| + "
               "|%2.4fG| = X + Y + Z\n",
               sumOfAxis, xAxis, yAxis, zAxis);
    } else {
        printf("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02X\n", errorCode);
    }
}

static void makeSelfTest() {
    printf("Start self test:\n");
    int delta_x, delta_y, delta_z;
    adxl345b_errorCode errorCode =
        adxl345b_performSelfTest(&delta_x, &delta_y, &delta_z);
    if (errorCode == ADXL345B_NO_ERROR) {
        printf("  \033[0;32mPASSED\033[0m\n");
    } else {
        printf("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02x\n", errorCode);
    }

    printf("  X: %iLSB, Y: %iLSB, Z: %iLSB\n", delta_x, delta_y, delta_z);
}

static void runCalibration() {
    printf("Start Calibration:\n");
    adxl345b_errorCode errorCode = adxl345b_runSelfCalibration();
    if (errorCode == ADXL345B_NO_ERROR) {
        printf("  \033[0;32mSUCCESSFUL\033[0m\n");
    } else {
        printf("  \033[0;31mFAILED\033[0m; adxl345b_ERROR: %02x\n", errorCode);
    }
}

static void enterBootMode() { reset_usb_boot(0, 0); }

int main(void) {
    /* enable print to console */
    stdio_init_all();
    /* wait to initialize screen session by user */
    sleep_ms(5000);

    /* initialize ADXL345B sensor */
    adxl345b_errorCode errorCode;
    while (1) {
        errorCode = adxl345b_init(i2c0, ADXL345B_I2C_ALTERNATE_ADDRESS);
        if (errorCode == ADXL345B_NO_ERROR) {
            printf("Initialised ADXL345B.\n");
            break;
        }
        printf("Initialise ADXL345B failed; adxl345b_ERROR: %02x\n", errorCode);
        sleep_ms(500);
    }

    /* test function of ADXL345B */
    printf("Please enter to request g (G value), s (serialNo), t (self test), "
           "c (calibration) or b (Boot mode)\n");
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
            printf("Please enter to request g (G value), s (serialNo), t (self "
                   "test), c (calibration) or b (Boot mode)\n");
            break;
        }
    }

    return 0;
}
