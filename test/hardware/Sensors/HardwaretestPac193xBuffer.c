#define SOURCE_FILE "PAC193X-BUFFER"

#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "I2c.h"
#include "Pac193x.h"
#include "Pac193xTypedefs.h"

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

static void sensorTest(pac193xSensorConfiguration_t sensor) {
    pac193xErrorCode_t errorCode = pac193xStartAccumulation(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return;
    }

    /* wait to gather samples */
    sleep_ms(2000);

    pac193xEnergyMeasurements_t measurements;
    errorCode = pac193xReadEnergyForAllChannels(sensor, &measurements);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return;
    }
    errorCode = pac193XStopAccumulation(sensor);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return;
    }

    PRINT("Performed %lu Measurements:", measurements.numberOfAccumulatedValues);
    PRINT("  Channel 1: %4.6fWs", measurements.energyChannel1);
    PRINT("  Channel 2: %4.6fWs", measurements.energyChannel2);
    PRINT("  Channel 3: %4.6fWs", measurements.energyChannel3);
    PRINT("  Channel 4: %4.6fWs", measurements.energyChannel4);
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

    PRINT("===== START INIT PAC_1 =====");
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

    PRINT("===== START TEST =====");
    PRINT("Please press 1 (sensor 1), 2 (sensor 2) or b (Boot mode) for a task to perform.");
    while (1) {
        char input = getchar_timeout_us(10000000); /* 10 seconds timeout */

        switch (input) {
        case '1':
            sensorTest(sensor1);
            break;
        case '2':
            sensorTest(sensor2);
            break;
        case 'b':
            enterBootMode();
            break;
        default:
            PRINT(
                "Please press 1 (sensor 1), 2 (sensor 2) or b (Boot mode) for a task to perform.");
            break;
        }
    }
}
