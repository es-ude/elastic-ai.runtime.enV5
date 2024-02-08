/* IMPORTANT: This program only works with the logic design with middleware!
 *
 * IMPORTANT: To reach access the wifi-network the network credentials have to be updated!
 *
 * NOTE: To run this test, a server that serves the HTTPGet request is required.
 *       This server can be started by running the `HW-Test_Webserver.py` script
 *       in the `bitfile_scripts` folder.
 *       After starting the server, it shows an IP-address where it can be reached.
 *       This IP address needs to be used for the `baseUrl` field.
 *
 * NOTE: If you update the echo_server binary file you have to update the `configSize` field
 *       with the correct size of the file in bytes.
 *       This size can be determined by running `du -b <path_to_file>`.
 */

#define SOURCE_FILE "HWTEST-STUB"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "HTTP.h"
#include "Network.h"
#include "enV5HwController.h"
#include "stub.h"
#include "stub_defs.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <stdio.h>
#include <stdlib.h>

extern networkCredentials_t networkCredentials;

char baseUrl[] = "http://192.168.178.24:5000/getconfig";
char lengthUrl[] = "http://192.168.178.24:5000/length";

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

#if BYTES_MODEL_ID == 1
uint8_t acceloratorId[BYTES_MODEL_ID] = {0x01};
#else
uint8_t acceloratorId[BYTES_MODEL_ID] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
#endif

static void initHardware() {
    // Should always be called first thing to prevent unique behavior, like current leakage
    env5HwInit();

    /* Always release flash after use:
     *   -> FPGA and MCU share the bus to flash-memory.
     * Make sure this is only enabled while FPGA does not use it and release after use before
     * powering on, resetting or changing the configuration of the FPGA.
     * FPGA needs that bus during reconfiguration and **only** during reconfiguration.
     */
    flashInit(&spiConfiguration, csPin);

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }
}

static void loadConfigToFlashViaHttp(uint32_t sectorId) {
    espInit();
    PRINT("Try Connecting to WiFi");
    networkTryToConnectToNetworkUntilSuccessful();

    PRINT("Request Download Size");
    HttpResponse_t *length_response;
    HTTPGet(lengthUrl, &length_response);
    length_response->response[length_response->length] = '\0';
    int configSize = strtol((char *)length_response->response, NULL, 10);
    HTTPCleanResponseBuffer(length_response);
    PRINT("Length: %i", configSize);

    PRINT("Downloading HW configuration...");
    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaHttp(baseUrl, configSize, sectorId);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Download failed!");
        exit(EXIT_FAILURE);
    }
    PRINT("Download Successful.");
}

_Noreturn void runTest(void) {
    PRINT("===== START TEST =====");
    while (1) {
        char c = getchar_timeout_us(UINT32_MAX);

        switch (c) {
        case 'a':
            loadConfigToFlashViaHttp(1);
            break;
        case 'A':
            loadConfigToFlashViaHttp(3);
            break;
        case 'P':
            env5HwFpgaPowersOn();
            PRINT("FPGA powered ON");
            break;
        case 'p':
            env5HwFpgaPowersOff();
            PRINT("FPGA powered OFF");
            break;
        case 'd':
            PRINT("Deploy: %s",
                  modelDeploy(3 * FLASH_BYTES_PER_SECTOR, acceloratorId) ? "successful" : "failed");
            break;
        default:
            PRINT("Waiting ...");
        }
    }
}

int main() {
    initHardware();
    runTest();
}
