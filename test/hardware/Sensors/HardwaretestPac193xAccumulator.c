#define SOURCE_FILE "HW-PAC193X-STREAM"

#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdio_usb.h>

#include "CException.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "I2c.h"
#include "Pac193x.h"
#include "Sleep.h"

/* region I2C DEFINITION */
i2cConfiguration_t i2cConfig = {
    .i2cInstance = I2C_MODULE,
    .frequency = I2C_FREQUENCY_IN_HZ,
    .sdaPin = I2C_SDA_PIN,
    .sclPin = I2C_SCL_PIN,
};
/* endregion I2C DEFINITION */

/* region SENSOR DEFINITION */
static pac193xSensorConfiguration_t sensor = {
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
/* endregion SENSOR DEFINITION */

static void initializeCommunication(void) {
    env5HwControllerInit();

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
    CEXCEPTION_T e;
    while (1) {
        Try {
            pac193xInit(sensor);
            PRINT("Initialised PAC193X.\n");
            break;
        }
        Catch(e) {
            PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", e);
        }

        sleep_ms(500);
    }
}

_Noreturn static void runTest(void) {
    PRINT("===== START TEST =====");
    pac193xStartAccumulation(sensor);
    pac193xSetSampleRate(sensor, PAC193X_256_SAMPLES_PER_SEC);
    pac193xRefreshDataAndResetAccumulator(sensor);

    sleep_for_ms(1000);

    while (true) {
        pac193xRefreshData(sensor);
        sleep_for_ms(10);
        pac193xEnergyMeasurements_t measurements;
        CEXCEPTION_T e;
        Try {
            pac193xReadEnergyForAllChannels(sensor, &measurements);
            PRINT("Overflow: %b", measurements.overflow);
            PRINT("Got %lu values:\n\t%f\n\t%f\n\t%f\n\t%f", measurements.numberOfAccumulatedValues,
                  measurements.energyChannel1, measurements.energyChannel2,
                  measurements.energyChannel3, measurements.energyChannel4);
        }
        Catch(e) {
            PRINT("Error occurred: 0x%02X", e);
        }

        PRINT("Sleeping for 2 seconds");
        sleep_for_ms(2000);
    }
}

int main(void) {
    initializeCommunication();
    runTest();
}
