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
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "Protocol.h"
#include "echo_server.h"
#include "enV5HwController.h"
#include "pico/bootrom.h"

#include <hardware/spi.h>
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

bool flashing = false;

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

uint32_t sectorIdForConfig = 1;

static void initHardware() {
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
    env5HwFpgaPowersOff();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }
}

_Noreturn static void runTest() {
    freeRtosTaskWrapperTaskSleep(7500);
    env5HwLedsAllOn();
    int8_t counter = 0;

    env5HwFpgaPowersOn();
    freeRtosTaskWrapperTaskSleep(500);
    if (!echo_server_deploy()) {
        PRINT("Deploy failed!")
    }

    while (true) {
        if (flashing) {
            freeRtosTaskWrapperTaskSleep(500);
            continue;
        }

        uint8_t return_val = (uint8_t)echo_server_echo(counter);

        PRINT("%i, %i", return_val, counter)

        if (return_val == counter + 1) {
            env5HwLedsAllOff();
            sleep_ms(500);
        }

        env5HwLedsAllOn();
        sleep_ms(500);

        counter++;
    }
}

void receiveDownloadBinRequest(posting_t posting) {
    PRINT("RECEIVED FLASH REQUEST")
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
    downloadRequest->startSectorId = position;
}

/* What this function does:
 *   - add listener for download start command (MQTT)
 *      uart handle should only set flag -> download handled at task
 *   - download data from server and stored to flash
 *   - add listener for FPGA flashing command
 *   - trigger flash of FPGA
 *      handled in UART interrupt
 */
_Noreturn void fpgaTask(void) {
    freeRtosTaskWrapperTaskSleep(5000);
    protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = receiveDownloadBinRequest});
    publishAliveStatusMessage("");

    PRINT("FPGA Ready ...")
    while (true) {
        if (downloadRequest == NULL) {
            freeRtosTaskWrapperTaskSleep(1000);
            continue;
        }
        flashing = true;

        PRINT("Starting to download bitfile...")

        env5HwFpgaPowersOff();

        PRINT_DEBUG("Download: position in flash: %i, address: %s, size: %i",
                    downloadRequest->startSectorId, downloadRequest->url,
                    downloadRequest->fileSizeInBytes)

        fpgaConfigurationHandlerError_t configError =
            fpgaConfigurationHandlerDownloadConfigurationViaHttp(downloadRequest->url,
                                                                 downloadRequest->fileSizeInBytes,
                                                                 downloadRequest->startSectorId);

        // clean artifacts
        free(downloadRequest->url);
        free(downloadRequest);
        downloadRequest = NULL;
        PRINT("Download finished!")

        PRINT("Try reconfigure FPGA")
        if (configError != FPGA_RECONFIG_NO_ERROR) {
            protocolPublishCommandResponse("FLASH", false);
            PRINT("ERASE ERROR")
        } else {
            freeRtosTaskWrapperTaskSleep(10);
            env5HwFpgaPowersOn();
            freeRtosTaskWrapperTaskSleep(500);
            if (!echo_server_deploy()) {
                PRINT("Deploy failed!")
            }
            flashing = false;
            PRINT("FPGA reconfigured")
            protocolPublishCommandResponse("FLASH", true);
        }
    }
}

int main() {
    initHardware();

    freeRtosTaskWrapperRegisterTask(fpgaTask, "fpgaTask", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(runTest, "runTest", 0, FREERTOS_CORE_1);

    freeRtosTaskWrapperStartScheduler();
}
