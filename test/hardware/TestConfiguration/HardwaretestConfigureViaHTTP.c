/* IMPORTANT: To reach access the wifi-network the network credentials have to be updated!
 *
 * NOTE: To run this test, a server that serves the HTTPGet request is required.
 *       This server can be started by running the `HW-Test_Webserver.py` script
 *       in the `bitfile_scripts` folder.
 *       After starting the server, it shows an IP-address where it can be reached.
 *       This IP address needs to be used for the `baseUrl` field.
 *
 * NOTE: If you update the binary files used in this test, you have to update the `configSize` field
 *       with the correct size of the file in bytes.
 *       This size can be determined by running `du -b <path_to_file>`.
 */

#define SOURCE_FILE "CONFIGURE-HWTEST"

#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Network.h"
#include "Spi.h"
#include "enV5HwController.h"

/* ES-Stud config */
spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

const char *baseUrl = "http://127.0.0.1:5000";
uint32_t blinkFast = 1;
size_t blinkFastLength = 86116;
uint32_t blinkSlow = 1;
size_t blinkSlowLength = 85540;

void initHardwareTest(void) {
    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // connect to Wi-Fi network
    espInit();
    networkTryToConnectToNetworkUntilSuccessful();

    // initialize the Flash and FPGA
    flashInit(&spiConfiguration, csPin);
    env5HwInit();
    fpgaConfigurationHandlerInitialize();
}

void downloadConfiguration(bool useFast) {
    fpgaConfigurationHandlerError_t error;
    char url[125];
    strcpy(url, baseUrl);

    if (useFast) {
        strcat(url, "/getfast");
        PRINT_DEBUG("URL: %s", url);

        error =
            fpgaConfigurationHandlerDownloadConfigurationViaHttp(url, blinkFastLength, blinkFast);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }
    } else {
        strcat(url, "/getslow");
        PRINT_DEBUG("URL: %s", url);

        error =
            fpgaConfigurationHandlerDownloadConfigurationViaHttp(url, blinkSlowLength, blinkSlow);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }
    }
    PRINT("Download Successful!");
}
void readConfiguration(bool useFast) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = (blinkFast - 1) * FLASH_BYTES_PER_SECTOR;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / FLASH_BYTES_PER_PAGE);
    } else {
        startAddress = (blinkSlow - 1) * FLASH_BYTES_PER_SECTOR;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / FLASH_BYTES_PER_PAGE);
    }

    data_t pageBuffer = {.data = NULL, .length = FLASH_BYTES_PER_PAGE};
    do {
        pageBuffer.data = calloc(FLASH_BYTES_PER_PAGE, sizeof(uint8_t));
        flashReadData(startAddress + (page * FLASH_BYTES_PER_PAGE), &pageBuffer);
        PRINT_BYTE_ARRAY("Configuration: ", pageBuffer.data, pageBuffer.length);
        free(pageBuffer.data);
        page++;
    } while (page < numberOfPages);
}
void verifyConfiguration(bool useFast) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = (blinkFast - 1) * FLASH_BYTES_PER_SECTOR;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / FLASH_BYTES_PER_PAGE);
    } else {
        startAddress = (blinkSlow - 1) * FLASH_BYTES_PER_SECTOR;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / FLASH_BYTES_PER_PAGE);
    }

    data_t pageBuffer = {.data = NULL, .length = FLASH_BYTES_PER_PAGE};
    stdio_flush();

    do {
        int input = getchar_timeout_us(UINT32_MAX);
        if (input == PICO_ERROR_TIMEOUT) {
            continue;
        }

        pageBuffer.data = calloc(FLASH_BYTES_PER_PAGE, sizeof(uint8_t));
        flashReadData(startAddress + (page * FLASH_BYTES_PER_PAGE), &pageBuffer);

        for (size_t index = 0; index < FLASH_BYTES_PER_PAGE; index++) {
            printf("%02X", pageBuffer.data[index]);
        }

        free(pageBuffer.data);
        page++;
    } while (page < numberOfPages);
}

_Noreturn void configurationTest(void) {
    PRINT("===== START TEST =====");

    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'E':
            flashEraseAll();
            PRINT("ERASED");
            break;
        case 'P':
            env5HwFpgaPowersOn();
            PRINT("FPGA Power: ON");
            break;
        case 'p':
            env5HwFpgaPowersOff();
            PRINT("FPGA Power: OFF");
            break;
        case 'd':
            downloadConfiguration(true);
            break;
        case 'r':
            readConfiguration(true);
            break;
        case 'v':
            verifyConfiguration(true);
            break;
        case 'D':
            downloadConfiguration(false);
            break;
        case 'R':
            readConfiguration(false);
            break;
        case 'V':
            verifyConfiguration(false);
            break;
        default:
            PRINT("Waiting ...");
        }
    }
}

int main() {
    initHardwareTest();
    configurationTest();
}
