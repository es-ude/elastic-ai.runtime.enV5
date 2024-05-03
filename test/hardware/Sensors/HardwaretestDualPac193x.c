#define SOURCE_FILE "DUAL-PAC193X-Test"

#include "Common.h"
#include "Pac193x.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <stdio.h>
#include "I2c.h"

/* region HELPER */

static void measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(sensor, channel, PAC193X_VSOURCE, &measurement);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return;
    }
    PRINT("VSource=%4.6fV", measurement);
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


/* region SENSOR DEFINITIONS */
static pac193xSensorConfiguration_t sensor1 = {
    .i2c_host = i2c1,
    .i2c_slave_address = PAC193X_I2C_ADDRESS_499R,
    .powerPin = -1,
    .usedChannels = {.uint_channelsInUse = 0b00001111},
    .rSense = {0.82f, 0.82f, 0.82f, 0.82f},
};
#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01
#define PAC193X_CHANNEL_RAW PAC193X_CHANNEL02
#define PAC193X_CHANNEL_MCU PAC193X_CHANNEL03
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL04

static pac193xSensorConfiguration_t sensor2 = {
    .i2c_host = i2c1,
    .i2c_slave_address = PAC193X_I2C_ADDRESS_806R,
    .powerPin = -1,
    .usedChannels = {.uint_channelsInUse = 0b00001111},
    .rSense = {0.82f, 0.82f, 0.82f, 0.82f},
};
#define PAC193X_CHANNEL_FPGA_IO PAC193X_CHANNEL01
#define PAC193X_CHANNEL_FPGA_1V8 PAC193X_CHANNEL02
#define PAC193X_CHANNEL_FPGA_1V PAC193X_CHANNEL03
#define PAC193X_CHANNEL_FPGA_SRAM PAC193X_CHANNEL04

/* endregion SENSOR DEFINITIONS */

static void getValuesFromSensor1() {
    PRINT("Requesting measurements for sensor 1.");
    PRINT("  Measure channel SENSOR:");
    measureValue(sensor1, PAC193X_CHANNEL_SENSORS);
    PRINT("  Measure channel RAW:");
    measureValue(sensor1, PAC193X_CHANNEL_RAW);
    PRINT("  Measure channel MCU:");
    measureValue(sensor1, PAC193X_CHANNEL_MCU);
    PRINT("  Measure channel WIFI:");
    measureValue(sensor1, PAC193X_CHANNEL_WIFI);
}

static void getValuesFromSensor2() {
    PRINT("Requesting measurements for sensor 2.");
    PRINT("  Measure channel FPGA IO:");
    measureValue(sensor1, PAC193X_CHANNEL_FPGA_IO);
    PRINT("  Measure channel FPGA 1V8:");
    measureValue(sensor1, PAC193X_CHANNEL_FPGA_1V8);
    PRINT("  Measure channel FPGA 1V:");
    measureValue(sensor1, PAC193X_CHANNEL_FPGA_1V);
    PRINT("  Measure channel FPGA SRAM:");
    measureValue(sensor1, PAC193X_CHANNEL_FPGA_SRAM);
}

static void getSerialNumber() {
    pac193xSensorId_t sensorID;

    PRINT("Requesting serial number of sensor 1.");
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

    PRINT("Requesting serial number of sensor 2");
    errorCode = pac193xGetSensorInfo(sensor2, &sensorID);
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
    PRINT("START I2C INIT");
    i2cErrorCode_t i2cErrorCode;
    while(1) {
        i2cErrorCode = i2cInit(&i2cConfig);
        if (i2cErrorCode == I2C_NO_ERROR){
            PRINT("Initialised I2C.");
            break;
        }
        PRINT("Initialise I2C failed; i2c_ERROR: %02X", i2cErrorCode);
        sleep_ms(500);
    }
    
    /* initialize PAC193X sensors */
    PRINT("===== START PAC_1 INIT =====");
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(sensor1);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.\n");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }

    PRINT("===== START PAC_2 INIT =====");
    errorCode;
    while (1) {
        errorCode = pac193xInit(sensor2);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 2.\n");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }

    PRINT("===== START TEST =====");
    PRINT("Please enter i (product ID), 1 (sensor 1), 2 (sensor 2) or b (Boot mode) for a task to "
          "perform.");
    while (1) {
        char input = getchar_timeout_us(10000000); /* 10 seconds timeout */

        switch (input) {
        case 'i':
            getSerialNumber();
            break;
        case '1':
            getValuesFromSensor1();
            break;
        case '2':
            getValuesFromSensor2();
            break;
        case 'b':
            enterBootMode();
            break;
        default:
            PRINT("Please enter i (product ID), 1 (sensor 1), 2 (sensor 2) or b (Boot mode) for a "
                  "task to perform.");
            break;
        }
    }

    return 0;
}
