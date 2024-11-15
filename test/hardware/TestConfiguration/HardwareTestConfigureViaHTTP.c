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

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Network.h"

#include "pico/stdio_usb.h"
#include "pico/stdlib.h"

#include "hardware/spi.h"

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};
flashConfiguration_t flashConfig;

void initializeFlashConfig() {
    flashConfig.spiConfiguration = &spiToFlashConfig;
    flashInit(&flashConfig);
    flashConfig.bytesPerPage = flashGetBytesPerPage(NULL);
    flashConfig.bytesPerSector = flashGetBytesPerSector(NULL);
    PRINT_DEBUG("Flash Config initialized.");
}

const char *baseUrl = "http://127.0.0.1:5000";
uint32_t blinkFast = 1;
size_t blinkFastLength = 86116;
uint32_t blinkSlow = 1;
size_t blinkSlowLength = 85540;

void initHardwareTest(void) {
    stdio_init_all();

    sleep_ms(1000);

    printf("0\n");

    env5HwControllerInit();
    printf("1\n");
    env5HwControllerFpgaPowersOff();
    printf("2\n");
    initializeFlashConfig();
    printf("3\n");

    while (!stdio_usb_connected()) {
        // wait for serial connection
    }

    espInit();
    networkTryToConnectToNetworkUntilSuccessful();

    fpgaConfigurationHandlerInitialize();
}

void downloadConfigurationHTTP(bool useFast) {
    fpgaConfigurationHandlerError_t error;
    char url[125];
    strcpy(url, baseUrl);

    if (useFast) {
        strcat(url, "/getfast");
        PRINT_DEBUG("URL: %s", url);

        error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(&flashConfig, url,
                                                                     blinkFastLength, blinkFast);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }
    } else {
        strcat(url, "/getslow");
        PRINT_DEBUG("URL: %s", url);

        error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(&flashConfig, url,
                                                                     blinkSlowLength, blinkSlow);
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
        startAddress = (blinkFast - 1) * flashConfig.bytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / flashConfig.bytesPerPage);
    } else {
        startAddress = (blinkSlow - 1) * flashConfig.bytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / flashConfig.bytesPerPage);
    }

    data_t pageBuffer = {.data = NULL, .length = flashConfig.bytesPerPage};
    do {
        pageBuffer.data = calloc(flashConfig.bytesPerPage, sizeof(uint8_t));
        flashReadData(&flashConfig, startAddress + (page * flashConfig.bytesPerPage), &pageBuffer);
        PRINT_BYTE_ARRAY("Configuration: ", pageBuffer.data, pageBuffer.length);
        free(pageBuffer.data);
        page++;
    } while (page < numberOfPages);
}
void verifyConfiguration(bool useFast) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = (blinkFast - 1) * flashConfig.bytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / flashConfig.bytesPerPage);
    } else {
        startAddress = (blinkSlow - 1) * flashConfig.bytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / flashConfig.bytesPerPage);
    }

    data_t pageBuffer = {.data = NULL, .length = flashConfig.bytesPerPage};
    stdio_flush();

    do {
        int input = getchar_timeout_us(UINT32_MAX);
        if (input == PICO_ERROR_TIMEOUT) {
            continue;
        }

        pageBuffer.data = calloc(flashConfig.bytesPerPage, sizeof(uint8_t));
        flashReadData(&flashConfig, startAddress + (page * flashConfig.bytesPerPage), &pageBuffer);

        for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
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
            flashEraseAll(NULL);
            PRINT("ERASED");
            break;
        case 'P':
            env5HwControllerFpgaPowersOn();
            PRINT("FPGA Power: ON");
            break;
        case 'p':
            env5HwControllerFpgaPowersOff();
            PRINT("FPGA Power: OFF");
            break;
        case 'd':
            downloadConfigurationHTTP(true);
            break;
        case 'r':
            readConfiguration(true);
            break;
        case 'v':
            verifyConfiguration(true);
            break;
        case 'D':
            downloadConfigurationHTTP(false);
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
