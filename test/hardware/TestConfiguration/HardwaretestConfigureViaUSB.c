//TODO: Improve stability
//      -> when generating a lot of output between downloads of config files it works!!

#define SOURCE_FILE "CONFIGURE-HWTEST"

#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Spi.h"
#include "enV5HwController.h"

spi_t spiConfiguration = {
    .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

uint32_t blinkFast = 0x00000000;
size_t blinkFastLength = 86116;
// uint32_t blinkSlow = 0x00045200;
uint32_t blinkSlow = 0x00000000;
size_t blinkSlowLength = 85540;

void initHardwareTest(void) {
    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    // initialize the Flash and FPGA
    flashInit(&spiConfiguration, csPin);
    env5HwInit();
    fpgaConfigurationHandlerInitialize();
}

void downloadConfiguration(bool useFast) {
    uint32_t startAddress;
    if (useFast) {
        startAddress = blinkFast;
    } else {
        startAddress = blinkSlow;
    }

    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaUsb(startAddress);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Error 0x%02X occurred during download.", error)
        return;
    }
    PRINT("Download Successful!")
}
void readConfiguration(bool useFast) {
    size_t numberOfPages = 1, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = blinkFast;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / FLASH_BYTES_PER_PAGE);
    } else {
        startAddress = blinkSlow;
        numberOfPages = (size_t)ceilf((float)blinkSlowLength / FLASH_BYTES_PER_PAGE);
    }

    data_t pageBuffer = {.data = NULL, .length = FLASH_BYTES_PER_PAGE};
    while (page < numberOfPages) {
        pageBuffer.data = calloc(FLASH_BYTES_PER_PAGE, sizeof(uint8_t));
        flashReadData(startAddress + (page * FLASH_BYTES_PER_PAGE), &pageBuffer);
        PRINT("Address: 0x%06lX", startAddress + (page * FLASH_BYTES_PER_PAGE))
        PRINT_BYTE_ARRAY("Configuration: ", pageBuffer.data, pageBuffer.length)
        free(pageBuffer.data);
        page++;
    }
}
void verifyConfiguration(bool useFast) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;
    if (useFast) {
        startAddress = blinkFast;
        numberOfPages = (size_t)ceilf((float)blinkFastLength / FLASH_BYTES_PER_PAGE);
    } else {
        startAddress = blinkSlow;
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
void configureFpga(uint32_t startAddress) {
    fpgaConfigurationHandlerError_t error = fpgaConfigurationFlashFpga(startAddress);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Reconfiguration failed! (0x%02X)", error)
        return;
    }
    PRINT("Reconfiguration successful!")
}
_Noreturn void configurationTest(void) {
    PRINT("===== START TEST =====")

    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'E':
            flashEraseAll();
            PRINT("ERASED!")
            break;
        case 'P':
            env5HwFpgaPowersOn();
            PRINT("FPGA Power: ON")
            break;
        case 'p':
            env5HwFpgaPowersOff();
            PRINT("FPGA Power: OFF")
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
        case 'f':
            configureFpga(blinkFast);
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
        case 'F':
            configureFpga(blinkSlow);
            break;
        default:
            PRINT("Waiting ...")
        }
    }
}

int main() {
    initHardwareTest();
    configurationTest();
}
