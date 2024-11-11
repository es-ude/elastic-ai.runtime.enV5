#define SOURCE_FILE "DUAL-PAC193X-Test"

#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>

#include "CException.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "I2c.h"
#include "Pac193x.h"

/* region HELPER */

static void measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;
    CEXCEPTION_T e;

    Try {
        measurement = pac193xGetMeasurementForChannel(&sensor, channel, PAC193X_VSOURCE);
    }

    Catch(e) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", e);
    }
    PRINT("VSource=%4.6fV", measurement);
}

/* endregion HELPER */

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = I2C_MODULE,
    .frequency = I2C_FREQUENCY_IN_HZ,
    .sdaPin = I2C_SDA_PIN,
    .sclPin = I2C_SCL_PIN,
};
/* endregion I2C DEFINITION */

/* region SENSOR DEFINITIONS */
static pac193xSensorConfiguration_t sensor1 = {
    .i2c_host = PAC_ONE_I2C_MODULE,
    .i2c_slave_address = PAC_ONE_SLAVE,
    .powerPin = PAC_ONE_POWER_PIN,
    .usedChannels = PAC_ONE_USED_CHANNELS,
    .rSense = PAC_ONE_R_SENSE,
};
#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01
#define PAC193X_CHANNEL_RAW PAC193X_CHANNEL02
#define PAC193X_CHANNEL_MCU PAC193X_CHANNEL03
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL04

static pac193xSensorConfiguration_t sensor2 = {
    .i2c_host = PAC_TWO_I2C_MODULE,
    .i2c_slave_address = PAC_TWO_SLAVE,
    .powerPin = PAC_TWO_POWER_PIN,
    .usedChannels = PAC_TWO_USED_CHANNELS,
    .rSense = PAC_TWO_R_SENSE,
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
    CEXCEPTION_T e;

    PRINT("Requesting serial number of sensor 1.");

    Try {
        sensorID = pac193xGetSensorInfo(&sensor1);
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
    }

    Catch(e) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", e);
    }

    PRINT("Requesting serial number of sensor 2");

    Try {
        sensorID = pac193xGetSensorInfo(&sensor2);
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
    }
    Catch(e) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", e);
    }
}

static void enterBootMode() {
    reset_usb_boot(0, 0);
}

int main(void) {

    CEXCEPTION_T e;
    /* enable print to console */
    stdio_init_all();
    // wait for user console to connect
    while ((!stdio_usb_connected())) {}
    sleep_ms(500);

    /* initialize I2C */
    PRINT("START I2C INIT");
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

    /* initialize PAC193X sensors */
    PRINT("===== START PAC_1 INIT =====");
    while (1) {
        Try {
            pac193xInit(&sensor1);
            PRINT("Initialised PAC193X sensor 1.\n");
            break;
        }
        Catch(e) {
            PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", e);
        }

        sleep_ms(500);
    }

    PRINT("===== START PAC_2 INIT =====");
    while (1) {
        Try {
            pac193xInit(&sensor2);
            PRINT("Initialised PAC193X sensor 2.\n");
            break;
        }
        Catch(e) {
            PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", e);
        }

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
}
