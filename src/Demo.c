#define SOURCE_FILE "MAIN"

// internal headers
#include "Common.h"
#include "Env5Hw.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHttp.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "MqttBroker.h"
#include "Network.h"
#include "NetworkConfiguration.h"
#include "Pac193x.h"
#include "Protocol.h"
#include "Spi.h"
#include "adxl345b/include/Adxl345b.h"

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
    bool (*whenSubscribed)(char *dataID);
    bool subscribed;
    uint8_t frequency;
    uint32_t lastPublished;
} receiver_t;
receiver_t *receivers[5];
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
void addDataRequestReceiver(receiver_t *receiver);
void addCommandRequestReceiver(receiver_t receiver);

void receiveDataStartRequest(posting_t posting);
void receiveDataStopRequest(posting_t posting);
bool getAndPublishSRamValue(char *dataID);
bool getAndPublishWifiValue(char *dataID);
bool getAndPublishGValueBatch(char *dataID);

void receiveDownloadBinRequest(posting_t posting);
void receiveFlashFpgaRequest(posting_t posting);

HttpResponse_t *getResponse(uint32_t block_number);

/* endregion MQTT */

/* endregion HEADER */

bool newBatch;
char *gValueDataBatch;
receiver_t gValueReceiver;

_Noreturn void batchTest() {

    PRINT("TEST")
    
    uint16_t batchSize = 100;
    uint8_t seconds = 5;
    uint32_t interval = seconds * 1000000;

    newBatch = false;
    gValueDataBatch = malloc(seconds * 11 * batchSize * 3 + 16);

    adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_BW_RATE, 0b00001010);
    adxl345bChangeMeasurementRange(ADXL345B_16G_RANGE);
    
    uint32_t count;
    
    while (1) {
        if (!gValueReceiver.subscribed) {
            freeRtosTaskWrapperTaskSleep(500);
            continue;
        }
        char *data = malloc(seconds * 11 * batchSize * 3 + 16);
        char timeBuffer[15];
        snprintf(timeBuffer, sizeof(timeBuffer), "%llu", time_us_64() / 1000000);
        strcpy(data, timeBuffer);
        strcat(data, ",");
        count = 0;

        uint32_t currentTime = time_us_64();
        uint32_t startTime = time_us_64();
        while (startTime + interval >= currentTime) {
            currentTime = time_us_64();
            if (count >= batchSize * seconds)
                continue;
            float xAxis, yAxis, zAxis;
            adxl345bErrorCode_t errorCode = adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);
            if (errorCode != ADXL345B_NO_ERROR) {
                PRINT("ERROR in Measuring G Value!")
            }

            char xBuffer[10];
            char yBuffer[10];
            char zBuffer[10];
            snprintf(xBuffer, sizeof(xBuffer), "%.10f", xAxis / 8);
            snprintf(yBuffer, sizeof(yBuffer), "%.10f", yAxis / 8);
            snprintf(zBuffer, sizeof(zBuffer), "%.10f", zAxis / 8);

            strcat(data, xBuffer);
            strcat(data, ",");
            strcat(data, yBuffer);
            strcat(data, ",");
            strcat(data, zBuffer);
            strcat(data, ";");
            count += 1;
        }
        newBatch = true;
        strcpy(gValueDataBatch, data);
        free(data);
    }
}

int main() {
    init();

    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 5);
    freeRtosTaskWrapperRegisterTask(batchTest, "batchTest", 1);
    freeRtosTaskWrapperRegisterTask(fpgaTask, "fpgaTask", 1);
    freeRtosTaskWrapperRegisterTask(sensorTask, "sensorTask", 2);
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

    i2c_set_baudrate(i2c1, 2000000);
    errorCode = adxl345bInit(i2c1, ADXL345B_I2C_ALTERNATE_ADDRESS);
    if (errorCode == ADXL345B_NO_ERROR)
        PRINT("Initialised ADXL345B.")
    else
        PRINT("Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode)

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
        PRINT("Download finished!")

        // reset FPGA
        freeRtosTaskWrapperTaskSleep(10);
        spiDeinit(FLASH_SPI, FLASH_CS, FLASH_SCK, FLASH_MOSI, FLASH_MISO);
        // load bitfile to FPGA
        env5HwFpgaPowersOn();
        PRINT("FPGA reconfigured")

        protocolPublishCommandResponse("FLASH", true);
    }
}

_Noreturn void sensorTask(void) {
    addDataRequestReceiver(
        &(receiver_t){.dataID = "wifi", .whenSubscribed = getAndPublishWifiValue, .frequency = 3});
    addDataRequestReceiver(
        &(receiver_t){.dataID = "sram", .whenSubscribed = getAndPublishSRamValue, .frequency = 3});
    gValueReceiver = (receiver_t){
        .dataID = "g-value", .whenSubscribed = getAndPublishGValueBatch, .frequency = 1};
    addDataRequestReceiver(&gValueReceiver);
    publishAliveStatusMessage("wifi,sram,g-value");

    PRINT("Ready ...")

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
                              receivers[i]->dataID)
                        receivers[i]->lastPublished = seconds;
                    }
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
        receivers[i]->subscribed = false;
    }
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

void addDataRequestReceiver(receiver_t *receiver) {
    receiver->subscribed = false;

    protocolSubscribeForDataStartRequest(receiver->dataID,
                                         (subscriber_t){.deliver = receiveDataStartRequest});
    protocolSubscribeForDataStopRequest(receiver->dataID,
                                        (subscriber_t){.deliver = receiveDataStopRequest});

    receivers[receivers_count] = receiver;
    receivers_count++;
}

bool getAndPublishSRamValue(char *dataID) {
    char buffer[64];
    float channelSensorValue = measureValue(powersensor2, PAC193X_CHANNEL_FPGA_SRAM);
    snprintf(buffer, sizeof(buffer), "%f", channelSensorValue);
    protocolPublishData(dataID, buffer);
    return true;
}

bool getAndPublishWifiValue(char *dataID) {
    char buffer[64];
    float channelWifiValue = measureValue(powersensor1, PAC193X_CHANNEL_WIFI);
    snprintf(buffer, sizeof(buffer), "%f", channelWifiValue);
    protocolPublishData(dataID, buffer);
    return true;
}

bool getAndPublishGValueBatch(char *dataID) {
    if (!newBatch)
        return false;
    protocolPublishData(dataID, gValueDataBatch);
    newBatch = false;
    return true;
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
