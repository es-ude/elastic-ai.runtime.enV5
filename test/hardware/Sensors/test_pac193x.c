#define SOURCE_FILE "PAC193X-Test"

#include "common.h"
#include "pac193x_public.h"
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

/* endregion */

#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL02

float resistanceValues[4] = {0.82f, 0.82f, 0, 0};
pac193x_usedChannels usedChannels = {.uint_channelsInUse = 0b00000011};

static void getValuesOfChannelWifi() {
    pac193x_measurements measurements;

    PRINT("Requesting measurements for wifi board.")
    pac193x_errorCode errorCode =
        pac193x_getAllMeasurementsForChannel(PAC193X_CHANNEL_WIFI, &measurements);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
        return;
    }

    PRINT("  Measurements:\tVSource=%4.6fV\tVSense=%4.6fmV\tISense=%4.6fmA",
          measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000)

    PRINT("  RSense_expected=%4.2fOhm, RSense_actual=%4.2fOhm:", resistanceValues[1],
          measurements.voltageSense / (measurements.iSense))
    if (compareFloatsWithinRange(resistanceValues[0],
                                 measurements.voltageSense / measurements.iSense, 0.1f)) {
        PRINT("    \033[0;32mPASSED\033[0m")
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Resistance values do not match!")
    }

    PRINT(
        "  Measured Power => %4.6fW; Calculated Power = Voltage_source * Current_Sense => %4.6fW:",
        measurements.powerActual, measurements.iSense * measurements.voltageSource)
    if (compareFloatsWithinRange(measurements.powerActual,
                                 measurements.iSense * measurements.voltageSource, 0.001f)) {
        PRINT("    \033[0;32mPASSED\033[0m")
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Values do not match!")
    }

    PRINT("  Energy = %4.6fWs", measurements.energy)
}

static void getValuesOfChannelSensors() {
    pac193x_measurements measurements;

    PRINT("Requesting measurements for sensors.")
    pac193x_errorCode errorCode =
        pac193x_getAllMeasurementsForChannel(PAC193X_CHANNEL_SENSORS, &measurements);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
        return;
    }

    PRINT("  Measurements:\tVSource=%4.6fV;\tVSense=%4.6fmV;\tISense=%4.6fmA",
          measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000)

    PRINT("  RSense_expected=%4.2fOhm, RSense_actual=%4.2fOhm:", resistanceValues[1],
          measurements.voltageSense / (measurements.iSense))
    if (compareFloatsWithinRange(resistanceValues[0],
                                 measurements.voltageSense / measurements.iSense, 0.1f)) {
        PRINT("    \033[0;32mPASSED\033[0m")
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Resistance values do not match!")
    }

    PRINT(
        "  Measured Power => %4.6fW; Calculated Power = Voltage_Source * Current_Sense => %4.6fW:",
        measurements.powerActual, measurements.iSense * measurements.voltageSource)
    if (compareFloatsWithinRange(measurements.powerActual,
                                 measurements.iSense * measurements.voltageSource, 0.001f)) {
        PRINT("    \033[0;32mPASSED\033[0m")
    } else {
        PRINT("    \033[0;31mFAILED\033[0m; Values do not match!")
    }

    PRINT("  Energy = %4.6fWs", measurements.energy)
}

static void getSerialNumber() {
    pac193x_info sensorID;

    PRINT("Requesting serial number.")
    pac193x_errorCode errorCode = pac193x_getSensorInfo(&sensorID);
    if (errorCode == PAC193X_NO_ERROR) {
        PRINT(
            "  Expected: Product ID: 0x%02X to 0x%02X; Manufacture ID: 0x%02X; Revision ID: 0x%02X",
            0x58, 0x5B, 0x5D, 0x03)
        PRINT("    Actual: Product ID: 0x%02X; Manufacture ID: 0x%02X; Revision ID: 0x%02X",
              sensorID.product_id, sensorID.manufacturer_id, sensorID.revision_id)

        bool valid_product_id = (sensorID.product_id >= 0x58) && (sensorID.product_id <= 0x5B);
        if (valid_product_id && 0x5D == sensorID.manufacturer_id && 0x03 == sensorID.revision_id) {
            PRINT("    \33[0;32mPASSED\033[0m")
        } else {
            PRINT("    \033[0;31mFAILED\033[0m; IDs do not match!")
        }
    } else {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
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

    /* initialize PAC193X sensor */
    PRINT("===== START INIT =====")
    pac193x_errorCode errorCode;
    while (1) {
        errorCode = pac193x_init(i2c1, resistanceValues, usedChannels);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X.\n")
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode)
        sleep_ms(500);
    }

    PRINT("===== START TEST =====")
    PRINT("Please enter to request i (product ID), w (channel wifi), s "
          "(channel sensor) or b (Boot mode)")
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
                  "s (channel sensor) or b (Boot mode)")
            break;
        }
    }

    return 0;
}
