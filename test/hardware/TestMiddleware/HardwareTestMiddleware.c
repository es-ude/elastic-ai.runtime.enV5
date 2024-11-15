/* IMPORTANT: This program only works with the logic design with middleware!
 *
 * IMPORTANT: To reach access to the wifi-network the network credentials have to be updated!
 *
 * NOTE: To run this test, a server that serves the HTTPGet requests is required.
 *       This server can be started by running the `HW-Test_Webserver.py` script
 *       in the `bitfile_scripts` folder.
 *       After starting the server, it shows an IP-address where it can be reached.
 *       This IP address needs to be used for the `baseUrl` and `lengthUrl` field.
 *
 * NOTE: The configuration to upload has to be copied as `config.bin` in the root folder.
 */

#define SOURCE_FILE "MIDDLEWARE-HWTEST"
#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"

#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "HTTP.h"
#include "Network.h"
#include "middleware.h"

#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "hardware/spi.h"

#include <stdlib.h>

char baseUrl[] = "http://192.168.178.24:5000/getconfig";
char lengthUrl[] = "http://192.168.178.24:5000/length";
uint32_t sectorIdForConfig = 1;

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};
flashConfiguration_t flashConfig = {.spiConfiguration = &spiToFlashConfig};

void initializeFlashConfig() {
    flashInit(&flashConfig);
    PRINT_DEBUG("Flash Config initialized.");
}

static void initializeCommunication(void) {
    // Should always be called first thing to prevent unique behavior, like current leakage
    env5HwControllerInit();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    /* Always release flash after use:
     *   -> FPGA and MCU share the bus to flash-memory.
     * Make sure this is only enabled while FPGA does not use it and release after use before
     * powering on, resetting or changing the configuration of the FPGA.
     * FPGA needs that bus during reconfiguration and **only** during reconfiguration.
     */

    espInit(); // initialize Wi-Fi chip
    networkTryToConnectToNetworkUntilSuccessful();
}

void downloadBinFile(void) {
    PRINT("Request Download Size");
    HttpResponse_t *length_response;
    HTTPGet(lengthUrl, &length_response);
    length_response->response[length_response->length] = '\0';
    int file_length = strtol((char *)length_response->response, NULL, 10);
    HTTPCleanResponseBuffer(length_response);
    PRINT("Length: %i", file_length);

    PRINT("Downloading HW configuration...");
    fpgaConfigurationHandlerError_t error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
        &flashConfig, baseUrl, file_length, sectorIdForConfig);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Download failed!");
        exit(EXIT_FAILURE);
    }
    PRINT("Download Successful.");
}

void getId(void) {
    PRINT("Request ID");
    uint8_t id[1];
    middlewareReadBlocking(2000, id, sizeof(id));
    PRINT("ID: 0x%02X", id[0]);
}

void writeData(void) {
    uint8_t data_write[3] = {0x01, 0x02, 0x03};
    middlewareWriteBlocking(0, data_write, 3);
    PRINT("Data written");
    uint8_t data_read[3];
    middlewareReadBlocking(0, data_read, 1);
    PRINT_BYTE_ARRAY("DATA: ", data_read, 3);
}

_Noreturn static void runTest() {
    PRINT("===== START TEST =====");
    while (1) {
        char c = getchar_timeout_us(UINT32_MAX);

        switch (c) {
        case 'C':
            downloadBinFile();
            break;
        case 'P':
            env5HwControllerFpgaPowersOn();
            PRINT("FPGA powered ON");
            break;
        case 'p':
            env5HwControllerFpgaPowersOff();
            PRINT("FPGA powered OFF");
            break;
        case 'I':
            middlewareInit();
            PRINT("INIT");
            break;
        case 'i':
            middlewareDeinit();
            PRINT("DEINIT");
            break;
        case 'L':
            middlewareSetFpgaLeds(0x09); // ON
            PRINT("LEDS ON");
            break;
        case 'l':
            middlewareSetFpgaLeds(0x00); // OFF
            PRINT("LEDS OFF");
            break;
        case 'U':
            middlewareUserlogicEnable();
            PRINT("Userlogic enabled");
            break;
        case 'u':
            middlewareUserlogicDisable();
            PRINT("Userlogic disabled");
            break;
        case 'd':
            getId();
            break;
        case 'r':
            // TODO: run reconfigure test
            break;
        case 't':
            writeData();
            break;
        default:
            PRINT("Waiting ...");
        }
    }
}

int main() {
    initializeCommunication();
    runTest();
}
