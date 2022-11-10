#define SOURCE_FILE "MAIN"

// src headers
#include "Common.h"
#include "Esp.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "I2c.h"
#include "MqttBroker.h"
#include "Network.h"
#include "NetworkConfiguration.h"
#include "Pac193x.h"
#include "hardware/i2c.h"

// external headers
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

#define PAC193X_CHANNEL_SENSORS PAC193X_CHANNEL01
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL02

float resistanceValues[4] = {0.82f, 0.82f, 0, 0};
pac193xUsedChannels_t usedChannels = {.uint_channelsInUse = 0b00000011};

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

static void getValuesOfChannelSensors() {
    pac193xMeasurements_t measurements;
    
    PRINT("Requesting measurements for sensors.")
    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(PAC193X_CHANNEL_SENSORS, &measurements);
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

static void getValuesOfChannelWifi() {
    pac193xMeasurements_t measurements;

    PRINT("Requesting measurements for wifi board.")
    pac193xErrorCode_t errorCode =
        pac193xGetAllMeasurementsForChannel(PAC193X_CHANNEL_WIFI, &measurements);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
        return;
    }

    PRINT("Measurements:\tVSource=%4.6fV\tVSense=%4.6fmV\tISense=%4.6fmA",
          measurements.voltageSource, measurements.voltageSense * 1000, measurements.iSense * 1000)

    PRINT("RSense_expected=%4.2fOhm, RSense_actual=%4.2fOhm:", resistanceValues[1],
          measurements.voltageSense / (measurements.iSense))
    if (compareFloatsWithinRange(resistanceValues[0],
                                 measurements.voltageSense / measurements.iSense, 0.1f)) {
        PRINT("\033[0;32mPASSED\033[0m")
    } else {
        PRINT("\033[0;31mFAILED\033[0m; Resistance values do not match!")
    }

    PRINT(
        "Measured Power => %4.6fW; Calculated Power = Voltage_source * Current_Sense => %4.6fW:",
        measurements.powerActual, measurements.iSense * measurements.voltageSource)
    if (compareFloatsWithinRange(measurements.powerActual,
                                 measurements.iSense * measurements.voltageSource, 0.001f)) {
        PRINT("\033[0;32mPASSED\033[0m")
    } else {
        PRINT("\033[0;31mFAILED\033[0m; Values do not match!")
    }

    PRINT("Energy = %4.6fWs", measurements.energy)
}

_Noreturn void mainTask(void) {
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
    mqttBrokerConnectToBrokerUntilSuccessful(mqttHost, "eip://uni-due.de/es", "enV5");

    PRINT("Initializing PAC193X...");
    
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(i2c1, resistanceValues, usedChannels);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X.")
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X", errorCode)
        sleep_ms(500);
    }
    
    //        while (1) {
    //            errorCode = pac193xInit(i2c1, resistanceValues, usedChannels);
    //            if (errorCode == PAC193X_NO_ERROR) {
    //                PRINT("Initialised PAC193X.\n")
    //                break;
    //            }
    //            PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode)
    //            sleep_ms(500);
    //        }
    
    while (true) {
        getValuesOfChannelWifi();
        sleep_ms(1000);
    }
}

// Goes into bootloader mode when 'r' is pressed
_Noreturn void enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            reset_usb_boot(0, 0);
        }
        // Watchdog update needs to be performed frequent, otherwise the device
        // will crash
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

void init(void) {
    // First check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    // init usb, queue and watchdog
    stdio_init_all();
    // waits for usb connection, REMOVE to continue without waiting for
    // connection
    while ((!stdio_usb_connected()))
        ;
    // Checks connection to ESP and initializes
    espInit();
    // Create FreeRTOS task queue
    freeRtosQueueWrapperCreate();
    // enables watchdog to check for reboots
    watchdog_enable(2000, 1);
}

int main() {
    init();
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(mainTask, "mainTask");
    // Starts FreeRTOS tasks
    freeRtosTaskWrapperStartScheduler();
}
