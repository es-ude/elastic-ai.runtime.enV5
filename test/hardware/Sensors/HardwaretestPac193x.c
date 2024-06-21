#define SOURCE_FILE "PAC193X-Test"

#include "Common.h"
#include "I2c.h"
#include "Pac193x.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
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

/* endregion HELPER */

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = i2c1,
    .frequency = 400000,
    .sdaPin = 6,
    .sclPin = 7,
};
/* endregion I2C DEFINITION */

/* region SENSOR DEFINITION */
#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL04

static pac193xSensorConfiguration_t sensor1 = {
    .i2c_host = i2c1,
    .i2c_slave_address = PAC193X_I2C_ADDRESS_499R,
    .powerPin = -1,
    .usedChannels = {.uint_channelsInUse = 0b00001111},
    .rSense = {0.82f, 0.82f, 0.82f, 0.82f},
};
/* endregion SENSOR DEFINTION */

static void getValuesOfChannelWifi() {
    pac193xMeasurements_t measurements;

    PRINT("Requesting measurements for wifi board.");
    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(sensor1, PAC193X_CHANNEL_WIFI, &measurements);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return;
    }

    PRINT("  Measurements:\tVSource=%4.6fV\tVSense=%4.6fmV\tISense=%4.6fmA",
          measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000);

    PRINT("  RSense_expected=%4.2fOhm, RSense_actual=%4.2fOhm:", sensor1.rSense[1],
          measurements.voltageSense / (measurements.iSense));
    if (compareFloatsWithinRange(sensor1.rSense[0], measurements.voltageSense / measurements.iSense,
                                 0.1f)) {
        PRINT("    \033[0;32mPASSED\033[0m");
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Resistance values do not match!");
    }

    PRINT(
        "  Measured Power => %4.6fW; Calculated Power = Voltage_source * Current_Sense => %4.6fW:",
        measurements.powerActual, measurements.iSense * measurements.voltageSource);
    if (compareFloatsWithinRange(measurements.powerActual,
                                 measurements.iSense * measurements.voltageSource, 0.001f)) {
        PRINT("    \033[0;32mPASSED\033[0m");
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Values do not match!");
    }
}

static void getValuesOfChannelSensors() {
    pac193xMeasurements_t measurements;

    PRINT("Requesting measurements for sensors.");
    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(sensor1, PAC193X_CHANNEL_SENSORS, &measurements);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return;
    }

    PRINT("  Measurements:\tVSource=%4.6fV;\tVSense=%4.6fmV;\tISense=%4.6fmA",
          measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000);

    PRINT("  RSense_expected=%4.2fOhm, RSense_actual=%4.2fOhm:", sensor1.rSense[1],
          measurements.voltageSense / (measurements.iSense));
    if (compareFloatsWithinRange(sensor1.rSense[0], measurements.voltageSense / measurements.iSense,
                                 0.1f)) {
        PRINT("    \033[0;32mPASSED\033[0m");
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Resistance values do not match!");
    }

    PRINT(
        "  Measured Power => %4.6fW; Calculated Power = Voltage_Source * Current_Sense => %4.6fW:",
        measurements.powerActual, measurements.iSense * measurements.voltageSource);
    if (compareFloatsWithinRange(measurements.powerActual,
                                 measurements.iSense * measurements.voltageSource, 0.001f)) {
        PRINT("    \033[0;32mPASSED\033[0m");
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Values do not match!");
    }
}

static void getSerialNumber() {
    pac193xSensorId_t sensorID;

    PRINT("Requesting serial number.");
    pac193xErrorCode_t errorCode = pac193xGetSensorInfo(sensor1, &sensorID);
    if (errorCode == PAC193X_NO_ERROR) {
        PRINT(
            "  Expected: Product ID: 0x%02X to 0x%02X; Manufacture ID: 0x%02X; Revision ID: 0x%02X",
            0x58, 0x5B, 0x5D, 0x03);
        PRINT("    Actual: Product ID: 0x%02X; Manufacture ID: 0x%02X; Revision ID: 0x%02X",
              sensorID.product_id, sensorID.manufacturer_id, sensorID.revision_id);

        bool valid_product_id = (sensorID.product_id >= 0x58) && (sensorID.product_id <= 0x5B);
        if (valid_product_id && 0x5D == sensorID.manufacturer_id && 0x03 == sensorID.revision_id) {
            PRINT("    \33[0;32mPASSED\033[0m");
        } else {
            PRINT("    \033[0;31mFAILED\033[0m; IDs do not match!");
        }
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
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

    /* initialize PAC193X sensor */
    PRINT("===== START PAC193X INIT =====");
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(sensor1);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X.\n");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }

    PRINT("===== START TEST =====");
    PRINT("Please enter to request i (product ID), w (channel wifi), s "
          "(channel sensor) or b (Boot mode)");
    while (1) {
        char input = getchar_timeout_us(10000000); /* 10 seconds wait */

        switch (input) {
        case 'i':
            getSerialNumber();
            break;
        case 'w':
            getValuesOfChannelWifi();
            break;
        case 's':
            getValuesOfChannelSensors();
            break;
        case 'b':
            enterBootMode();
            break;
        default:
            PRINT("Please enter to request i (product ID), w (channel wifi), "
                  "s (channel sensor) or b (Boot mode)");
            break;
        }
    }

    return 0;
}
