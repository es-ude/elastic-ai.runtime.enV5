#define SOURCE_FILE "MAIN"

// internal headers
#include "Adxl345b.h"
#include "Common.h"
#include "Esp.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "MqttBroker.h"
#include "Pac193x.h"
#include "Protocol.h"
#include "Spi.h"

// pico-sdk headers
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

// external headers
#include <malloc.h>
#include <string.h>

/* region POWER-SENSOR 1 */

static pac193xSensorConfiguration_t powersensor1 = {
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

/* endregion POWER-SENSOR 1 */

/* region POWER-SENSOR 2 */

static pac193xSensorConfiguration_t powersensor2 = {
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

/* endregion POWER-SENSOR 2 */

void receiveDataStartRequest(posting_t posting);
void receiveDataStopRequest(posting_t posting);

typedef struct receiver {
    char *dataID;
    bool (*whenSubscribed)(char *dataID);
    bool subscribed;
    uint8_t frequency;
    uint32_t lastPublished;
    bool newValue;
    char *value;
} receiver_t;
receiver_t *receivers[5];
uint16_t receivers_count = 0;

receiver_t sramReceiver;
receiver_t wifiReceiver;

char *twinID;

void init(void) {
    // check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }

    // init IO
    stdio_init_all();
    // waits for usb connection, REMOVE to continue without waiting for connection
    while ((!stdio_usb_connected())) {}
    PRINT("");

    // initialize ESP over UART
    espInit();

    // initialize WiFi and MQTT broker
    connectToNetwork();
    connectToMQTT();

    // initialize power sensors
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(powersensor1);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }
    while (1) {
        errorCode = pac193xInit(powersensor2);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 2.");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }

    i2c_set_baudrate(i2c1, 2000000);
    errorCode = adxl345bInit(i2c1, ADXL345B_I2C_ALTERNATE_ADDRESS);
    if (errorCode == ADXL345B_NO_ERROR)
        PRINT("Initialised ADXL345B.");
    else
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode);

    // create FreeRTOS task queue
    freeRtosQueueWrapperCreate();

    // enables watchdog timer (5s)
    watchdog_enable(5000, 1);
}

void setTwinID(char *newTwinID) {
    if (newTwinID == twinID) {
        return;
    }

    if (newTwinID != NULL) {
        free(twinID);
    }

    twinID = malloc(strlen(newTwinID) + 1);
    strcpy(twinID, newTwinID);
}

void addDataRequestReceiver(receiver_t *receiver) {
    receiver->subscribed = false;

    protocolSubscribeForDataStartRequest(receiver->dataID,
                                         (subscriber_t){.deliver = receiveDataStartRequest});
    protocolSubscribeForDataStopRequest(receiver->dataID,
                                        (subscriber_t){.deliver = receiveDataStopRequest});

    receivers[receivers_count] = receiver;
    receivers_count++;
}

void receiveDataStartRequest(posting_t posting) {
    setTwinID(posting.data);

    for (int i = 0; i < receivers_count; ++i) {
        if (strstr(posting.topic, receivers[i]->dataID) != NULL) {
            receivers[i]->subscribed = true;
            break;
        }
    }
}

void receiveDataStopRequest(posting_t posting) {
    setTwinID(posting.data);

    for (int i = 0; i < receivers_count; ++i) {
        if (strstr(posting.topic, receivers[i]->dataID) != NULL) {
            receivers[i]->subscribed = false;
            break;
        }
    }
}

bool getAndPublishSRamValue(char *dataID) {
    if (!sramReceiver.newValue);
        return false;
    protocolPublishData(dataID, sramReceiver.value);
    sramReceiver.newValue = false;
    return true;
}

bool getAndPublishWifiValue(char *dataID) {
    if (!wifiReceiver.newValue)
        return false;
    protocolPublishData(dataID, wifiReceiver.value);
    wifiReceiver.newValue = false;
    return true;
}

void twinsIsOffline(posting_t posting) {
    if (strstr(posting.data, STATUS_STATE_ONLINE) != NULL) {
        return;
    }
    PRINT("Twin is Offline");

    for (int i = 0; i < receivers_count; ++i) {
        receivers[i]->subscribed = false;
    }
}

_Noreturn void publishValueBatchesTask(void) {
    addDataRequestReceiver(
        &(receiver_t){.dataID = "wifi", .whenSubscribed = getAndPublishWifiValue, .frequency = 5});
    sramReceiver =
        (receiver_t){.dataID = "sram", .whenSubscribed = getAndPublishSRamValue, .frequency = 3};
    addDataRequestReceiver(&sramReceiver);

    publishAliveStatusMessage("wifi,sram");

    PRINT("Ready ...");

    uint64_t seconds;
    bool hasTwin = false;
    while (true) {
        seconds = (time_us_64()) / 1000000;
        freeRtosTaskWrapperTaskSleep(100);

        bool toSomeTopicIsSubscribed = false;
        for (int i = 0; i < receivers_count; ++i) {
            if (receivers[i]->subscribed) {
                if (seconds - receivers[i]->lastPublished >= receivers[i]->frequency) {
                    if (receivers[i]->whenSubscribed(receivers[i]->dataID)) {
                        PRINT("Published Sensor Value (sec: %llu, data: %s)", seconds,
                              receivers[i]->dataID);
                        receivers[i]->lastPublished = seconds;
                    }
                }
                toSomeTopicIsSubscribed = true;
            }
        }

        if (!hasTwin && (toSomeTopicIsSubscribed)) {
            hasTwin = true;
            protocolSubscribeForStatus(twinID, (subscriber_t){.deliver = twinsIsOffline});
        }
        if (hasTwin && (!toSomeTopicIsSubscribed)) {
            hasTwin = false;
            protocolUnsubscribeFromStatus(twinID, (subscriber_t){.deliver = twinsIsOffline});
        }
    }
}

float measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(sensor, channel, PAC193X_VSOURCE_AVG, &measurement);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return -1;
    }
    PRINT("Measured Value: %4.2f", measurement);
    return measurement;
}

_Noreturn void getSRAMValue() {
    sramReceiver.newValue = false;
    sramReceiver.value = malloc(64);
    while (1) {
        if (!sramReceiver.subscribed) {
            freeRtosTaskWrapperTaskSleep(500);
            continue;
        }
        float channelSensorValue = measureValue(powersensor2, PAC193X_CHANNEL_FPGA_SRAM);
        snprintf(sramReceiver.value, sizeof(sramReceiver.value), "%.02f", channelSensorValue);
        sramReceiver.newValue = true;
        freeRtosTaskWrapperTaskSleep(3000);
    }
}

_Noreturn void getWifiValue() {
    wifiReceiver.newValue = false;
    wifiReceiver.value = malloc(64);
    while (1) {
        if (!wifiReceiver.subscribed) {
            freeRtosTaskWrapperTaskSleep(500);
            continue;
        }
        float channelSensorValue = measureValue(powersensor1, PAC193X_CHANNEL_WIFI);
        snprintf(wifiReceiver.value, sizeof(wifiReceiver.value), "%.02f", channelSensorValue);
        wifiReceiver.newValue = true;
        freeRtosTaskWrapperTaskSleep(3000);
    }
}

_Noreturn void enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            // enter boot mode if 'r' was received
            reset_usb_boot(0, 0);
        }

        // watchdog update needs to be performed frequent, otherwise the device will crash
        watchdog_update();
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

int main() {
    init();

    freeRtosTaskWrapperRegisterTask(getSRAMValue, "getSRAMValue", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(getWifiValue, "getWifiValue", 0, FREERTOS_CORE_0);

    freeRtosTaskWrapperRegisterTask(publishValueBatchesTask, "publishValueBatchesTask", 1,
                                    FREERTOS_CORE_1);
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 1, FREERTOS_CORE_1);

    freeRtosTaskWrapperStartScheduler();
}
