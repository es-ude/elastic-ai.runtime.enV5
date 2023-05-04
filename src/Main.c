#define SOURCE_FILE "MAIN"

// internal headers
#include "Common.h"
#include "Env5Hw.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHttp.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "NetworkConfiguration.h"
#include "Pac193x.h"
#include "Protocol.h"
#include "Spi.h"
#include "middleware.h"

// pico-sdk headers
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

// external headers
#include <malloc.h>
#include <string.h>

/* region VARIABLES/DEFINES */

/* region FLASH */

#define FLASH_SPI spi0
#define FLASH_SCK 2
#define FLASH_MISO 0
#define FLASH_MOSI 3
#define FLASH_CS 1
#define FLASH_BAUDRATE 5000000

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

void receiveDownloadBinRequest(posting_t posting);
void receiveFlashFpgaRequest(posting_t posting);

HttpResponse_t *getResponse(uint32_t block_number);

/* endregion MQTT */

/* endregion HEADER */

int main() {
    init();

    freeRtosTaskWrapperRegisterTask(fpgaTask, "fpgaTask");
    freeRtosTaskWrapperRegisterTask(sensorTask, "sensorTask");
    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask");
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

    env5HwInit();
    setCommunication(getResponse);

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
    /* NOTES:
     *   1. add listener for download start command (MQTT)
     *      uart handle should only set flag -> download handled at task
     *   2. download data from server and stored to flash
     *   4. add listener for FPGA flashing command
     *   5. trigger flash of FPGA
     *      handled in UART interrupt
     */

    setCommunication(getResponse);

    freeRtosTaskWrapperTaskSleep(5000);
    protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = receiveDownloadBinRequest});

    PRINT("Ready ...")

    while (1) {
        if (downloadRequest == NULL) {
            freeRtosTaskWrapperTaskSleep(1000);
            continue;
        }
        
        env5HwFpgaPowersOff();

        // initialize SPI, flash and FPGA
        spiInit(FLASH_SPI, FLASH_BAUDRATE, FLASH_CS, FLASH_SCK, FLASH_MOSI, FLASH_MISO);
        flashInit(FLASH_CS, FLASH_SPI);

        // download bitfile from server
        PRINT_DEBUG("Download: position in flash: %i, address: %s, size: %i",
                    downloadRequest->startAddress, downloadRequest->url,
                    downloadRequest->fileSizeInBytes)
        if (configure(downloadRequest->startAddress, downloadRequest->fileSizeInBytes) ==
            CONFIG_ERASE_ERROR) {
            PRINT("ERASE ERROR")
            protocolPublishCommandResponse("FLASH", false);
        }
        free(downloadRequest->url);
        free(downloadRequest);
        downloadRequest = NULL;

        //        // reset FPGA
        //        env5HwFpgaReset(1);
        freeRtosTaskWrapperTaskSleep(10);
        //        env5HwFpgaReset(0);
        spiDeinit(FLASH_SPI, FLASH_CS, FLASH_SCK, FLASH_MOSI, FLASH_MISO);
        // load bitfile to FPGA
        env5HwFpgaPowersOn();
        PRINT_DEBUG("reconfig done")

        protocolPublishCommandResponse("FLASH", true);
    }
}

_Noreturn void sensorTask(void) {
    addDataRequestReceiver(
        (receiver_t){.dataID = "wifi", .whenSubscribed = getAndPublishWifiValue});
    addDataRequestReceiver(
        (receiver_t){.dataID = "sram", .whenSubscribed = getAndPublishSRamValue});
    publishAliveStatusMessage("wifi,sram");

    PRINT("Ready ...")

    bool hasTwin = false;
    while (true) {
        bool toSomeTopicIsSubscribed = false;
        for (int i = 0; i < receivers_count; ++i) {
            if (receivers[i].subscribed) {
                receivers[i].whenSubscribed(receivers[i].dataID);
                toSomeTopicIsSubscribed = true;
            }
            freeRtosTaskWrapperTaskSleep(500);
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

HttpResponse_t *getResponse(uint32_t block_number) {
    char *blockNo = malloc(10 * sizeof(char));
    sprintf(blockNo, "%li", block_number);

    char *URL = malloc(strlen(downloadRequest->url) + 1 + strlen(blockNo));
    strcpy(URL, downloadRequest->url);
    strcat(URL, "/");
    strcat(URL, blockNo);

    HttpResponse_t *response;
    uint8_t code = HTTPGet(URL, &response);
    PRINT_DEBUG("HTTP Get returns with %u", code);
    PRINT_DEBUG("Response Length: %li", response->length)

    free(blockNo);
    free(URL);
    return response;
}

/* endregion PROTOTYPE IMPLEMENTATIONS */
