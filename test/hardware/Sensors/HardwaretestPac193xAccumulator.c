#define SOURCE_FILE "HW-PAC193X-STREAM"

#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdio_usb.h>

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "I2c.h"
#include "Pac193x.h"
#include "Sleep.h"

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = I2C_INSTANCE,
    .frequency = I2C_FREQUENCY_IN_HZ,
    .sdaPin = I2C_SDA_PIN,
    .sclPin = I2C_SCL_PIN,
};
/* endregion I2C DEFINITION */

/* region SENSOR DEFINITION */
static pac193xSensorConfiguration_t sensor1 = {
    .i2c_host = PAC_ONE_HOST,
    .i2c_slave_address = PAC_ONE_SLAVE,
    .powerPin = PAC_ONE_POWER_PIN,
    .usedChannels = PAC_ONE_USED_CHANNELS,
    .rSense = PAC_ONE_R_SENSE,
};

#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01
#define PAC193X_CHANNEL_RAW PAC193X_CHANNEL02
#define PAC193X_CHANNEL_MCU PAC193X_CHANNEL03
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL04
/* endregion SENSOR DEFINITION */

static void initHardware(void) {
    /* enable print to console */
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for user console to connect
    }
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
}

_Noreturn static void runTest(void) {
    PRINT("===== START TEST =====");
    pac193xStartAccumulation(sensor1);
    pac193xSetSampleRate(sensor1, PAC193X_256_SAMPLES_PER_SEC);

    while (true) {
        pac193xRefreshData(sensor1);
        pac193xEnergyMeasurements_t measurements;
        pac193xErrorCode_t error = pac193xReadEnergyForAllChannels(sensor1, &measurements);
        if (PAC193X_NO_ERROR != error) {
            PRINT("Got %lu values:\n\t%f\n\t%f\n\t%f\n\t%f", measurements.numberOfAccumulatedValues,
                  measurements.energyChannel1, measurements.energyChannel2,
                  measurements.energyChannel3, measurements.energyChannel4);
        } else {
            PRINT("Error occurred: 0x%02X", error);
        }
        PRINT("Sleeping for 2 seconds");
        sleep_for_ms(2000);
    }
}

int main(void) {
    initHardware();
    runTest();
}
