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
    size_t startSectorId;
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

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
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
downloadRequest_t *parseDownloadRequest(char *data) {
    PRINT_DEBUG("RECEIVED FLASH REQUEST");

    /* region parse url */
    char *urlStart = strstr(data, "URL:") + 4;
    char *urlEnd = strstr(urlStart, ";") - 1;
    size_t urlLength = urlEnd - urlStart + 1;
    char *url = malloc(urlLength);
    strncpy(url, urlStart, urlLength);
    /* endregion parse url */
    /* region parse length */
    char *sizeStart = strstr(data, "SIZE:") + 5;
    char *endSize = strstr(sizeStart, ";") - 1;
    size_t length = strtol(sizeStart, &endSize, 10);
    /* endregion parse length */
    /* region parse sector-ID */
    char *positionStart = strstr(data, "POSITION:") + 9;
    char *positionEnd = strstr(positionStart, ";") - 1;
    size_t position = strtol(positionStart, &positionEnd, 10);
    /* endregion parse sector-ID */

    downloadRequest_t *downloadRequest = malloc(sizeof(downloadRequest_t));
    downloadRequest->url = url;
    downloadRequest->fileSizeInBytes = length;
    downloadRequest->startSectorId = position;
    return downloadRequest;
}
_Noreturn void handleReceiveTasks(void) {
    protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = deliver});

    while (1) {
        posting_t post;
        if (freeRtosQueueWrapperPop(postings, &post)) {
            PRINT_DEBUG("Received Message: '%s' via '%s'", post.data, post.topic);
            if (NULL != strstr(post.topic, "DO/FLASH")) {
                downloadRequest_t *downloadRequest = parseDownloadRequest(post.data);
                freeRtosQueueWrapperPush(testCases, downloadRequest);
                free(post.topic);
                free(post.data);
            }
        }
        freeRtosTaskWrapperTaskSleep(1000);
    }
}

_Noreturn void handlePublishTask(void) {
    while (1) {
        publishRequest_t request;
        if (freeRtosQueueWrapperPop(publishRequests, &request)) {
            switch (request.pubType) {
            case COMMAND_RESPONSE:
                freeRtosMutexWrapperLock(espOccupied);
                protocolPublishCommandResponse(request.topic, strcmp(request.data, "SUCCESS"));
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

bool successfullyDownloadBinFile(downloadRequest_t *downloadRequest) {
    freeRtosMutexWrapperLock(espOccupied);
    fpgaConfigurationHandlerError_t error =
        FPGA_RECONFIG_NO_ERROR !=
        fpgaConfigurationHandlerDownloadConfigurationViaHttp(
            downloadRequest.url, downloadRequest.fileSizeInBytes, downloadRequest.startSectorId);
    freeRtosMutexWrapperUnlock(espOccupied);
    publishRequest_t downloadDonePosting;
    downloadDonePosting.pubType = COMMAND_RESPONSE;
    downloadDonePosting.topic = calloc(6, sizeof(char));
    strcpy(downloadDonePosting.topic, "FLASH");
    if (FPGA_RECONFIG_NO_ERROR != error) {
        downloadDonePosting.data = calloc(7, sizeof(char));
        strcpy(downloadDonePosting.data, "FAILED");
        return false;
    }
    downloadDonePosting.data = calloc(8, sizeof(char));
    strcpy(downloadDonePosting.data, "SUCCESS");
    freeRtosQueueWrapperPush(publishRequests, &downloadDonePosting);
    return true;
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
            if (!successfullyDownloadBinFile(&downloadRequest)) {
                continue;
            }
            env5HwFpgaPowersOn();
            freeRtosTaskWrapperTaskSleep(100);
            middlewareConfigureFpga(downloadRequest.startSectorId);
            blinkFpgaLeds();
        }
    }
}

int main() {
    initHardware();

    postings = freeRtosQueueWrapperCreate(5, sizeof(posting_t));
    publishRequests = freeRtosQueueWrapperCreate(5, sizeof(downloadRequest_t));
    testCases = freeRtosQueueWrapperCreate(5, sizeof(size_t));

    espOccupied = freeRtosMutexWrapperCreate();
    freeRtosMutexWrapperUnlockFromInterrupt(espOccupied);

    publishAliveStatusMessageWithMandatoryAttributes((status_t){});

    freeRtosTaskWrapperRegisterTask(handleReceiveTasks, "receive", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(handlePublishTask, "send", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(runTestTask, "test", 1, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
