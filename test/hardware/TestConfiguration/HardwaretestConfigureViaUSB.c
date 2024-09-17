/* IMPORTANT: This script is for test-purposes only, because it lacks reliability!
 *
 * NOTE: To run this test, a python script, that transmits the requested binary file is required.
 *       This script can be started by running the `BinfileFlasher.py` script
 *       in the `bitfile_scripts` folder.
 *       To run this script all other serial connections (USB) to the enV5 device have to be
 *       closed.
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

#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Spi.h"

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};
flashConfiguration_t flashConfig;

void initializeFlashConfig() {
    flashConfig.flashSpiConfiguration = &spiToFlashConfig;
    flashInit(&flashConfig);
    flashConfig.flashBytesPerPage = flashGetBytesPerPage();
    flashConfig.flashBytesPerSector = flashGetBytesPerSector();
    PRINT_DEBUG("Flash Config initialized.");
}

uint32_t blinkFast = 1;
size_t blinkFastLength = 86116;
uint32_t blinkSlow = 1;
size_t blinkSlowLength = 85540;

void initHardwareTest(void) {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    // initialize the serial output
    stdio_init_all();
    initializeFlashConfig();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // initialize FPGA
    fpgaConfigurationHandlerInitialize();
}

void downloadConfiguration(bool useFast) {
    uint32_t sectorID;
    if (useFast) {
        sectorID = blinkFast;
    } else {
        sectorID = blinkSlow;
    }

    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaUsb(&flashConfig, sectorID);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Error 0x%02X occurred during download.", error);
        return;
    }
    PRINT("Download Successful!");
}
void readConfiguration(bool useFast) {
    size_t numberOfPages = 1, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = (blinkFast - 1) * flashConfig.flashBytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / flashConfig.flashBytesPerPage);
    } else {
        startAddress = (blinkSlow - 1) * flashConfig.flashBytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / flashConfig.flashBytesPerPage);
    }

    data_t pageBuffer = {.data = NULL, .length = flashConfig.flashBytesPerPage};
    while (page < numberOfPages) {
        pageBuffer.data = calloc(flashConfig.flashBytesPerPage, sizeof(uint8_t));
        flashReadData(&flashConfig, startAddress + (page * flashConfig.flashBytesPerPage), &pageBuffer);
        PRINT("Address: 0x%06lX", startAddress + (page * flashConfig.flashBytesPerPage));
        PRINT_BYTE_ARRAY("Configuration: ", pageBuffer.data, pageBuffer.length);
        free(pageBuffer.data);
        page++;
    }
}
void verifyConfiguration(bool useFast) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = (blinkFast - 1) * flashConfig.flashBytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / flashConfig.flashBytesPerPage);
    } else {
        startAddress = (blinkSlow - 1) * flashConfig.flashBytesPerSector;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / flashConfig.flashBytesPerPage);
    }

    data_t pageBuffer = {.data = NULL, .length = flashConfig.flashBytesPerPage};
    stdio_flush();

    do {
        int input = getchar_timeout_us(UINT32_MAX);
        if (input == PICO_ERROR_TIMEOUT) {
            continue;
        }

        pageBuffer.data = calloc(flashConfig.flashBytesPerPage, sizeof(uint8_t));
        flashReadData(&flashConfig, startAddress + (page * flashConfig.flashBytesPerPage), &pageBuffer);

        for (size_t index = 0; index < flashConfig.flashBytesPerPage; index++) {
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
            PRINT("ERASED!");
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
