#define SOURCE_FILE "MAIN"

// internal headers
#include "Adxl345b.h"
#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "MqttBroker.h"
#include "Network.h"
#include "NetworkConfiguration.h"
#include "Pac193x.h"
#include "Protocol.h"
#include "Spi.h"
#include "enV5HwController.h"
#include "middleware.h"

// pico-sdk headers
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

// external headers
#include "CException.h"
#include <malloc.h>
#include <string.h>

/* region VARIABLES/DEFINES */

/* region FLASH */

spi_t spiConfigurationForFlash = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPinForFlash = 1;

/* endregion FLASH */

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

/* region MQTT */

char *twinID;

typedef struct receiver {
    char *dataID;
    void (*whenSubscribed)(char *dataID);
    bool subscribed;
    uint8_t frequency;
    uint32_t lastPublished;
} receiver_t;
receiver_t receivers[5];
uint16_t receivers_count = 0;

typedef struct downloadRequest {
    char *url;
    size_t fileSizeInBytes;
    size_t startAddress;
} downloadRequest_t;
downloadRequest_t *downloadRequest = NULL;

/* endregion MQTT */

/* endregion VARIABLES/DEFINES */

/* region PROTOTYPES */

/* region HARDWARE */

/// initialize hardware (watchdog, UART, ESP, WiFi, MQTT, I²C, sensors, ...)
void init(void);

/// retrieve single-shot value from power sensor
float measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel);

void flashFpga(uint8_t address);
void downloadBinFile();

/* endregion HARDWARE */

/* region FreeRTOS TASKS */

_Noreturn void fpgaTask(void);

_Noreturn void sensorTask(void);

/// goes into bootloader mode when 'r' is pressed
_Noreturn void enterBootModeTask(void);

/* endregion FreeRTOS TASKS */

/* region MQTT */

void setTwinID(char *newTwinID);
void offline(posting_t posting);
void addDataRequestReceiver(receiver_t receiver);
void addCommandRequestReceiver(receiver_t receiver);

void receiveDataStartRequest(posting_t posting);
void receiveDataStopRequest(posting_t posting);
void getAndPublishSRamValue(char *dataID);
void getAndPublishWifiValue(char *dataID);
void getAndPublishGValue(char *dataID);

void receiveDownloadBinRequest(posting_t posting);
void receiveFlashFpgaRequest(posting_t posting);

HttpResponse_t *getResponse(uint32_t block_number);

/* endregion MQTT */

/* endregion HEADER */

int main() {
    init();

    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask");
    freeRtosTaskWrapperRegisterTask(fpgaTask, "fpgaTask");
    freeRtosTaskWrapperRegisterTask(sensorTask, "sensorTask");
    freeRtosTaskWrapperStartScheduler();
}

/* region PROTOTYPE IMPLEMENTATIONS */

void init(void) {
    // check if we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }

    // init IO
    stdio_init_all();
    // waits for usb connection, REMOVE to continue without waiting for connection
    while ((!stdio_usb_connected())) {}
    PRINT("")

    // initialize ESP over UART
    espInit();

    // initialize WiFi and MQTT broker
    networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
    mqttBrokerConnectToBrokerUntilSuccessful(mqttHost, "eip://uni-due.de/es", "enV5");

    // initialize power sensors
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(powersensor1);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.")
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode)
        sleep_ms(500);
    }
    while (1) {
        errorCode = pac193xInit(powersensor2);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 2.")
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode)
        sleep_ms(500);
    }

    errorCode = adxl345bInit(i2c1, ADXL345B_I2C_ALTERNATE_ADDRESS);
    if (errorCode == ADXL345B_NO_ERROR)
        PRINT("Initialised ADXL345B.")
    else
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode)

    // initialize flash and FPGA
    flashInit(&spiConfigurationForFlash, csPinForFlash);
    env5HwFpgaInit();
    fpgaConfigurationHandlerInitialize();

    // create FreeRTOS task queue
    freeRtosQueueWrapperCreate();

    // enables watchdog timer (5s)
    watchdog_enable(5000, 1);
}

float measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(sensor, channel, PAC193X_VSOURCE_AVG, &measurement);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode)
        return -1;
    }
    PRINT("Measured Value: %4.2f", measurement)
    return measurement;
}

_Noreturn void fpgaTask(void) {
    /* NOTE:
     *   1. add listener for download start command (MQTT)
     *      uart handle should only set flag -> download handled at task
     *   2. download data from server and stored to flash
     *   4. add listener for FPGA flashing command
     *   5. trigger flash of FPGA
     *      handled in UART interrupt
     */

    freeRtosTaskWrapperTaskSleep(5000);
    protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = receiveDownloadBinRequest});

    PRINT("FPGA Ready ...")

    while (1) {
        if (downloadRequest == NULL) {
            freeRtosTaskWrapperTaskSleep(1000);
            continue;
        }

        env5HwFpgaPowersOff();

        PRINT_DEBUG("Download: position in flash: %i, address: %s, size: %i",
                    downloadRequest->startAddress, downloadRequest->url,
                    downloadRequest->fileSizeInBytes)
        if (fpgaConfigurationHandlerDownloadConfigurationViaHttp(
                downloadRequest->url, downloadRequest->fileSizeInBytes,
                downloadRequest->startAddress) == FPGA_RECONFIG_ERASE_ERROR) {
            PRINT("ERASE ERROR")
            protocolPublishCommandResponse("FLASH", false);
        }
        
        // clean artifacts
        free(downloadRequest->url);
        free(downloadRequest);
        downloadRequest = NULL;
        
        PRINT("Download finished!")
        freeRtosTaskWrapperTaskSleep(10);
        
        env5HwFpgaPowersOn();
        PRINT("FPGA reconfigured")

        protocolPublishCommandResponse("FLASH", true);
    }
}

_Noreturn void sensorTask(void) {
    addDataRequestReceiver(
        (receiver_t){.dataID = "wifi", .whenSubscribed = getAndPublishWifiValue, .frequency = 1});
    addDataRequestReceiver(
        (receiver_t){.dataID = "sram", .whenSubscribed = getAndPublishSRamValue, .frequency = 3});
    publishAliveStatusMessage("wifi,sram");

    PRINT("Ready ...")

    uint32_t seconds;
    bool hasTwin = false;
    while (true) {
        seconds = (time_us_32()) / 1000000;
        freeRtosTaskWrapperTaskSleep(250);

        bool toSomeTopicIsSubscribed = false;
        for (int i = 0; i < receivers_count; ++i) {
            if (receivers[i].subscribed) {
                if (seconds - receivers[i].lastPublished >= receivers[i].frequency) {
                    PRINT("sec: %lu, data: %s", seconds, receivers[i].dataID)
                    receivers[i].whenSubscribed(receivers[i].dataID);
                    receivers[i].lastPublished = seconds;
                }
                toSomeTopicIsSubscribed = true;
            }
        }

        if (!hasTwin && (toSomeTopicIsSubscribed)) {
            hasTwin = true;
            protocolSubscribeForStatus(twinID, (subscriber_t){.deliver = offline});
        }
        if (hasTwin && (!toSomeTopicIsSubscribed)) {
            hasTwin = false;
            protocolUnsubscribeFromStatus(twinID, (subscriber_t){.deliver = offline});
        }
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

void offline(posting_t posting) {
    if (strstr(posting.data, STATUS_STATE_ONLINE) != NULL) {
        return;
    }
    PRINT("Twin offline")

    for (int i = 0; i < receivers_count; ++i) {
        receivers[i].subscribed = false;
    }
}

void receiveDataStartRequest(posting_t posting) {
    setTwinID(posting.data);

    for (int i = 0; i < receivers_count; ++i) {
        if (strstr(posting.topic, receivers[i].dataID) != NULL) {
            receivers[i].subscribed = true;
            break;
        }
    }
}

void receiveDataStopRequest(posting_t posting) {
    setTwinID(posting.data);

    for (int i = 0; i < receivers_count; ++i) {
        if (strstr(posting.topic, receivers[i].dataID) != NULL) {
            receivers[i].subscribed = false;
            break;
        }
    }
}

void addDataRequestReceiver(receiver_t receiver) {
    receiver.subscribed = false;

    protocolSubscribeForDataStartRequest(receiver.dataID,
                                         (subscriber_t){.deliver = receiveDataStartRequest});
    protocolSubscribeForDataStopRequest(receiver.dataID,
                                        (subscriber_t){.deliver = receiveDataStopRequest});

    receivers[receivers_count] = receiver;
    receivers_count++;
}

void getAndPublishSRamValue(char *dataID) {
    char buffer[64];
    float channelSensorValue = measureValue(powersensor2, PAC193X_CHANNEL_FPGA_SRAM);
    snprintf(buffer, sizeof(buffer), "%f", channelSensorValue);
    protocolPublishData(dataID, buffer);
}

void getAndPublishWifiValue(char *dataID) {
    char buffer[64];
    float channelWifiValue = measureValue(powersensor1, PAC193X_CHANNEL_WIFI);
    snprintf(buffer, sizeof(buffer), "%f", channelWifiValue);
    protocolPublishData(dataID, buffer);
}

void getAndPublishGValue(char *dataID) {
    float xAxis, yAxis, zAxis;
    adxl345bErrorCode_t errorCode = adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);
    if (errorCode != ADXL345B_NO_ERROR) {
        PRINT("ERROR in Measuring G Value!")
        return;
    }
    float gValue = xAxis + yAxis + zAxis;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%f", gValue);
    protocolPublishData(dataID, buffer);
}

void receiveDownloadBinRequest(posting_t posting) {
    // get download request
    char *urlStart = strstr(posting.data, "URL:") + 4;
    char *urlEnd = strstr(urlStart, ";") - 1;
    size_t urlLength = urlEnd - urlStart + 1;
    char *url = malloc(urlLength);
    memcpy(url, urlStart, urlLength);
    url[urlLength - 1] = '\0';
    char *sizeStart = strstr(posting.data, "SIZE:") + 5;
    char *endSize = strstr(sizeStart, ";") - 1;
    size_t length = strtol(sizeStart, &endSize, 10);

    char *positionStart = strstr(posting.data, "POSITION:") + 9;
    char *positionEnd = strstr(positionStart, ";") - 1;
    size_t position = strtol(positionStart, &positionEnd, 10);

    downloadRequest = malloc(sizeof(downloadRequest_t));
    downloadRequest->url = url;
    downloadRequest->fileSizeInBytes = length;
    downloadRequest->startAddress = position;
}

/* endregion PROTOTYPE IMPLEMENTATIONS */
