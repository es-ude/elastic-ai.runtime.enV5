/* IMPORTANT: This program only works with the logic design with middleware!
 *
 * IMPORTANT: To reach access the wifi-network the network credentials have to be updated!
 *
 * NOTE: To run this test, the monitor is required!
 */

#define SOURCE_FILE "HWTEST-STUB-WITH-MONITOR"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "FreeRtosMutexWrapper.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Protocol.h"
#include "enV5HwController.h"
#include "middleware.h"

#include <hardware/spi.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct downloadRequest {
    char *url;
    size_t fileSizeInBytes;
    uint32_t startSectorId;
} downloadRequest_t;

typedef enum {
    COMMAND_RESPONSE,
    DATA_VALUE,
} pubType_t;
typedef struct publishRequest {
    pubType_t pubType;
    char *topic;
    char *data;
} publishRequest_t;

spiConfig_t spiConfiguration = {
    .spiInstance = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

queue_t postings;
queue_t publishRequests;
queue_t testCases;

mutex_t espOccupied;

void initHardware() {
    // Should always be called first thing to prevent unique behavior, like current leakage
    env5HwInit();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    espInit(); // initialize Wi-Fi chip
    networkTryToConnectToNetworkUntilSuccessful();
    mqttBrokerConnectToBrokerUntilSuccessful("eip://uni-due.de/es", "enV5");

    /* Always release flash after use:
     *   -> FPGA and MCU share the bus to flash-memory.
     * Make sure this is only enabled while FPGA does not use it and release after use before
     * powering on, resetting or changing the configuration of the FPGA.
     * FPGA needs that bus during reconfiguration and **only** during reconfiguration.
     */
    flashInit(&spiConfiguration, csPin);
    fpgaConfigurationHandlerInitialize();
}

void deliver(posting_t posting) {
    freeRtosQueueWrapperPushFromInterrupt(postings, &posting);
}
downloadRequest_t parseDownloadRequest(char *data) {
    PRINT("RECEIVED FLASH REQUEST");

    /* region parse length */
    char *sizeStart = strstr(data, "SIZE:") + 5;
    size_t length = strtol(sizeStart, NULL, 10);
    /* endregion parse length */
    /* region parse sector-ID */
    char *positionStart = strstr(data, "POSITION:") + 9;
    uint32_t position = strtol(positionStart, NULL, 10);
    /* endregion parse sector-ID */
    /* region parse url */
    char *urlStart = strstr(data, "URL:") + 4;
    char *urlRaw = strtok(urlStart, ";");
    size_t urlLength = strlen(urlRaw);
    char *url = malloc(urlLength + 1);
    strcpy(url, urlRaw);
    /* endregion parse url */

    PRINT_DEBUG("URL: %s", url);
    PRINT_DEBUG("LENGTH: %zu", length);
    PRINT_DEBUG("SECTOR 0: %lu", position);

    return (downloadRequest_t){.url = url, .startSectorId = position, .fileSizeInBytes = length};
}
_Noreturn void handleReceiveTasks(void) {
    freeRtosTaskWrapperTaskSleep(500);
    protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = deliver});

    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) {
            PRINT_DEBUG("Received Message: '%s' via '%s'", post.data, post.topic);
            if (NULL != strstr(post.topic, "DO/FLASH")) {
                downloadRequest_t downloadRequest = parseDownloadRequest(post.data);
                freeRtosQueueWrapperPush(testCases, &downloadRequest);
                free(post.topic);
                free(post.data);
            }
        }
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

_Noreturn void handlePublishTask(void) {
    publishAliveStatusMessageWithMandatoryAttributes((status_t){.fpga = "V1", .data = "test"});

    while (1) {
        publishRequest_t request;
        if (freeRtosQueueWrapperPop(publishRequests, &request)) {
            switch (request.pubType) {
            case COMMAND_RESPONSE:
                freeRtosMutexWrapperLock(espOccupied);
                protocolPublishCommandResponse(request.topic, strcmp(request.data, "FAILED"));
                freeRtosMutexWrapperUnlock(espOccupied);
                break;
            case DATA_VALUE:
                freeRtosMutexWrapperLock(espOccupied);
                protocolPublishData(request.topic, request.data);
                freeRtosMutexWrapperUnlock(espOccupied);
                break;
            default:
                PRINT_DEBUG("type NOT valid!");
            }

            free(request.topic);
            free(request.data);
        }
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

bool successfullyDownloadBinFile(char *url, size_t lengthOfBinFile, uint32_t startSector) {
    freeRtosMutexWrapperLock(espOccupied);
    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaHttp(url, lengthOfBinFile, startSector);
    freeRtosMutexWrapperUnlock(espOccupied);
    free(url);

    publishRequest_t downloadDonePosting;
    downloadDonePosting.pubType = COMMAND_RESPONSE;
    downloadDonePosting.topic = calloc(6, sizeof(char));
    strcpy(downloadDonePosting.topic, "FLASH");
    if (FPGA_RECONFIG_NO_ERROR == error) {
        downloadDonePosting.data = calloc(8, sizeof(char));
        strcpy(downloadDonePosting.data, "SUCCESS");
        freeRtosQueueWrapperPush(publishRequests, &downloadDonePosting);
        return true;
    }
    downloadDonePosting.data = calloc(7, sizeof(char));
    strcpy(downloadDonePosting.data, "FAILED");
    freeRtosQueueWrapperPush(publishRequests, &downloadDonePosting);
    return false;
}
void blinkFpgaLeds(void) {
    middlewareSetFpgaLeds(0b00000000);
    freeRtosTaskWrapperTaskSleep(500);
    middlewareSetFpgaLeds(0b00000001);
    freeRtosTaskWrapperTaskSleep(500);
    middlewareSetFpgaLeds(0b00000011);
    freeRtosTaskWrapperTaskSleep(500);
    middlewareSetFpgaLeds(0b00000111);
    freeRtosTaskWrapperTaskSleep(500);
    middlewareSetFpgaLeds(0b00001111);
    freeRtosTaskWrapperTaskSleep(1000);
    middlewareSetFpgaLeds(0b00000000);
    publishRequest_t testDonePosting;
    testDonePosting.pubType = DATA_VALUE;
    testDonePosting.topic = calloc(10, sizeof(char));
    strcpy(testDonePosting.topic, "FPGA/TEST");
    testDonePosting.data = calloc(5, sizeof(char));
    strcpy(testDonePosting.data, "DONE");
    freeRtosQueueWrapperPush(publishRequests, &testDonePosting);
}
_Noreturn void runTestTask(void) {
    while (1) {
        downloadRequest_t downloadRequest;
        if (freeRtosQueueWrapperPop(testCases, &downloadRequest)) {
            /* 1. Download config to flash
             * 2. Power on FPGA (load default config with middleware)
             * 3. Deploy new config via middleware
             * 4. Run Middleware test (Blink FPGA LED)
             */
            env5HwFpgaPowersOff();
            if (!successfullyDownloadBinFile(downloadRequest.url, downloadRequest.fileSizeInBytes,
                                             downloadRequest.startSectorId)) {
                continue;
            }
            env5HwFpgaPowersOn();
            //            freeRtosTaskWrapperTaskSleep(100);
            //            middlewareConfigureFpga(downloadRequest.startSectorId);
            //            blinkFpgaLeds();
            publishRequest_t testSuccess;
            testSuccess.pubType = DATA_VALUE;
            testSuccess.topic = calloc(6, sizeof(char));
            strcpy(testSuccess.topic, "test");
            testSuccess.data = calloc(8, sizeof(char));
            strcpy(testSuccess.data, "DONE");
            freeRtosQueueWrapperPush(publishRequests, &testSuccess);
        }
    }
}

int main() {
    initHardware();

    postings = freeRtosQueueWrapperCreate(5, sizeof(posting_t));
    publishRequests = freeRtosQueueWrapperCreate(5, sizeof(publishRequest_t));
    testCases = freeRtosQueueWrapperCreate(5, sizeof(downloadRequest_t));

    espOccupied = freeRtosMutexWrapperCreate();
    freeRtosMutexWrapperUnlockFromInterrupt(espOccupied);

    freeRtosTaskWrapperRegisterTask(handleReceiveTasks, "receive", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(handlePublishTask, "send", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(runTestTask, "test", 1, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
