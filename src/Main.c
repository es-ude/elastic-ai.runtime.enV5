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
#include "Protocol.h"
#include "hardware/i2c.h"

// external headers
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

bool wifiValueIsRequested = false;
bool sRAMValueIsRequested = false;

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

float measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(sensor, channel, PAC193X_VSOURCE_AVG, &measurement);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
        return -1;
    }
    return measurement;
}

void receiveWifiDataStartRequest(posting_t posting) {
    wifiValueIsRequested = true;
}

void receiveWifiDataStopRequest(posting_t posting) {
    wifiValueIsRequested = false;
}

void receiveSRAMDataStartRequest(posting_t posting) {
    sRAMValueIsRequested = true;
}

void receiveSRAMDataStopRequest(posting_t posting) {
    sRAMValueIsRequested = false;
}


_Noreturn void mainTask(void) {
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
    mqttBrokerConnectToBrokerUntilSuccessful(mqttHost, "eip://uni-due.de/es", "enV5");

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
    
    PRINT("===== INIT SENSOR 2 =====")
    while (1) {
        errorCode = pac193xInit(sensor2);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.\n")
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode)
        sleep_ms(500);
    }

    protocolSubscribeForDataStartRequest("wifiValue", (subscriber_t){.deliver =
                                                                         receiveWifiDataStartRequest});
    
    protocolSubscribeForDataStopRequest("wifiValue", (subscriber_t){.deliver =
                                                                         receiveWifiDataStopRequest});
    
    protocolSubscribeForDataStartRequest("sramValue",
                                         (subscriber_t){.deliver = receiveSRAMDataStartRequest});
    
    protocolSubscribeForDataStopRequest("sramValue",
                                        (subscriber_t){.deliver = receiveSRAMDataStopRequest});
    
    char buffer[64];
    
    while (true) {
        if (wifiValueIsRequested) {
            float channelWifiValue = measureValue(sensor1, PAC193X_CHANNEL_WIFI);
            snprintf(buffer, sizeof buffer, "%f", channelWifiValue);
            protocolPublishData("wifiValue", buffer);
        }
        sleep_ms(100);
        if (sRAMValueIsRequested) {
            float channelSensorValue = measureValue(sensor2, PAC193X_CHANNEL_FPGA_SRAM);
            snprintf(buffer, sizeof buffer, "%f", channelSensorValue);
            protocolPublishData("sRamValue", buffer);
        }
        sleep_ms(900);
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
