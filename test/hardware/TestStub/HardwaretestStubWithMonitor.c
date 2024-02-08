/* IMPORTANT: This program only works with the logic design with middleware!
 *
 * IMPORTANT: To reach access the wifi-network the network credentials have to be updated!
 *
 * NOTE: To run this test, the monitor is required!
 */

#define SOURCE_FILE "HWTEST-STUB"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "FreeRtosQueueWrapper.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Protocol.h"
#include "enV5HwController.h"

#include <hardware/spi.h>
#include <pico/bootrom.h>
#include <pico/stdlib.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern networkCredentials_t networkCredentials;
extern mqttBrokerHost_t mqttHost;

typedef struct downloadRequest {
    char *url;
    size_t fileSizeInBytes;
    size_t startSectorId;
} downloadRequest_t;
downloadRequest_t *downloadRequest = NULL;

volatile bool testInProgress = false;

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

void initHardware() {
    // Should always be called first thing to prevent unique behavior, like current leakage
    env5HwInit();

    // Connect to Wi-Fi network and MQTT Broker
    espInit();
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

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }
}

_Noreturn void runTest() {
    while (true) {
        /* TODO:
         *   1. Get next config to flash from queue
         *   2. Power on FPGA (load default config with middleware)
         *   3. Deploy new config via middleware
         *   4. Run Middleware test (Blink FPGA LED)
         */
    }
}

static void receiveDownloadBinRequest(posting_t posting) {
    PRINT("RECEIVED FLASH REQUEST");
    // get download request
    char *urlStart = strstr(posting.data, "URL:"); + 4;
    char *urlEnd = strstr(urlStart, ";"); - 1;
    size_t urlLength = urlEnd - urlStart + 1;
    char *url = malloc(urlLength);
    memcpy(url, urlStart, urlLength);
    url[urlLength - 1] = '\0';
    char *sizeStart = strstr(posting.data, "SIZE:"); + 5;
    char *endSize = strstr(sizeStart, ";"); - 1;
    size_t length = strtol(sizeStart, &endSize, 10);

    char *positionStart = strstr(posting.data, "POSITION:"); + 9;
    char *positionEnd = strstr(positionStart, ";"); - 1;
    size_t position = strtol(positionStart, &positionEnd, 10);

    downloadRequest = malloc(sizeof(downloadRequest_t));
    downloadRequest->url = url;
    downloadRequest->fileSizeInBytes = length;
    downloadRequest->startSectorId = position;
}

/* What this function does:
 *   - add listener for download start command (MQTT)
 *      uart handle should only set flag -> download handled at task
 *   - download data from server and stored to flash
 */
_Noreturn void downloadTask(void) {
    freeRtosTaskWrapperTaskSleep(5000);
    protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = receiveDownloadBinRequest});
    publishAliveStatusMessage("");

    PRINT("FPGA Ready ...");
    while (true) {
        if (downloadRequest == NULL || testInProgress) {
            freeRtosTaskWrapperTaskSleep(1000);
            continue;
        }

        // assure free SPI/Flash access
        env5HwFpgaPowersOff();

        PRINT("Starting to download bitfile...");
        PRINT_DEBUG("Download: position in flash: %i, address: %s, size: %i",
                    downloadRequest->startSectorId, downloadRequest->url,
                    downloadRequest->fileSizeInBytes);
        fpgaConfigurationHandlerError_t configError =
            fpgaConfigurationHandlerDownloadConfigurationViaHttp(downloadRequest->url,
                                                                 downloadRequest->fileSizeInBytes,
                                                                 downloadRequest->startSectorId);

        // clean artifacts
        free(downloadRequest->url);
        free(downloadRequest);
        downloadRequest = NULL;

        if (configError != FPGA_RECONFIG_NO_ERROR) {
            protocolPublishCommandResponse("FLASH", false);
            PRINT("ERASE ERROR");
        } else {
            // TODO: Push new reconfig request to queue
            protocolPublishCommandResponse("FLASH", true);
            PRINT("Download finished!");
        }
    }
}

int main() {
    initHardware();

    freeRtosQueueWrapperCreate();

    freeRtosTaskWrapperRegisterTask(downloadTask, "downloadTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(runTest, "runTest", 0, FREERTOS_CORE_1);

    freeRtosTaskWrapperStartScheduler();
}
