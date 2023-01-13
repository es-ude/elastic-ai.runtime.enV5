#include "Common.h"
#include "Pac193x.h"
#include "Pac193xTypedefs.h"
#include <hardware/i2c.h>
#include <pico/bootrom.h>
#include <pico/stdio_usb.h>
#include <stdio.h>

/* region HELPER */

static void measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;
    
    pac193xErrorCode_t errorCode =
                           pac193xGetMeasurementForChannel(sensor, channel, PAC193X_VSOURCE, &measurement);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
        return;
    }
    PRINT("VSource=%4.6fV", measurement)
}

/* endregion HELPER */

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

static void sensorTest() {
    // TODO: implement sensor test for background read
    
    // TODO: start continuous power accumulation
    // TODO: busy wait for x ms
    // TODO: read accumulated value along with counter
    // TODO: stop continuous power accumulation
    // TODO: print values to console
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
    
    PRINT("===== INIT SENSOR 1 =====")
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(sensor1);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.\n")
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode)
        sleep_ms(500);
    }
    
    PRINT("===== START TEST =====")
    PRINT("Please b (Boot mode) for a task to perform.")
    while (1) {
        char input = getchar_timeout_us(10000000); /* 10 seconds timeout */
        
        switch (input) {
        case 't':
            sensorTest();
        case 'b':
            enterBootMode();
            break;
        default:
            PRINT("Please b (Boot mode) for a task to perform.")
            break;
        }
    }
    
    return 0;
}
