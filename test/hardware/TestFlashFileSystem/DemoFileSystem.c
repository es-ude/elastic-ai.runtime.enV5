/*
IMPORTANT THINGS:

- each entry has 5 attributes: ID, start sector, size, isConfig and number of sectors
- first use initFileSystem() to check if file system exits and reconstruct it
  otherwise it just initializes all required values
- always use findFittingStartSector() to check if your file fits. If it does, the sector to be used
is stored in the variable nextFileSector
- nextFileSector is the number of the sector you should write your file to. This is to be converted
  into an actual address.

*/

#define SOURCE_FILE "DEMO_FILESYSTEM"

#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "FileSystem.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Network.h"

#include <Sleep.h>

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MISO,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS
};

flashConfiguration_t flashConfig = {
    .flashSpiConfiguration = &spiToFlashConfig,
    .flashBytesPerPage = FLASH_BYTES_PER_PAGE,
    .flashBytesPerSector = FLASH_BYTES_PER_SECTOR,
};

const char *baseUrl = "http://192.168.2.21:5000";
// const char *baseUrl = "http://192.168.203.223:5000";

size_t blinkFastLength = 86116;
size_t blinkSlowLength = 85540;

void initDemo() {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    stdio_init_all();
    while (!stdio_usb_connected()) {}

    espInit();
    networkTryToConnectToNetworkUntilSuccessful();

    sleep_for_ms(1000);
    PRINT("\n");
    PRINT("===== File System Demo =====\n");
}

void downloadConfigurationHTTP(bool useFast) {
    uint8_t numberOfRequiredSectors;
    fpgaConfigurationHandlerError_t error;
    char url[125];
    strcpy(url, baseUrl);

    if (useFast) {
        numberOfRequiredSectors =
            (uint8_t)ceilf((float)blinkFastLength / (float)FLASH_BYTES_PER_SECTOR);

        // STUFF FOR FILE SYSTEM ------------------------------------
        if (!filesystemFindFittingStartSector(numberOfRequiredSectors)) {
            return;
        }
        filesystemAddNewFileSystemEntry(&flashConfig, blinkFastLength, 1);
        // ----------------------------------------------------------

        strcat(url, "/getfast");
        PRINT_DEBUG("URL: %s", url);
        error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
            &flashConfig, url, blinkFastLength, nextFileSector);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }

    } else {
        numberOfRequiredSectors =
            (uint8_t)ceilf((float)blinkSlowLength / (float)FLASH_BYTES_PER_SECTOR);

        // STUFF FOR FILE SYSTEM -----------------------------------------
        if (!filesystemFindFittingStartSector(numberOfRequiredSectors)) {
            PRINT("Not enough space...\n Aborting...\n");
            return;
        }
        // ----------------------------------------------------------------

        strcat(url, "/getslow");
        PRINT_DEBUG("URL: %s", url);
        error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
            &flashConfig, url, blinkSlowLength, nextFileSector);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }
    }
    PRINT("Download Successful!");
    filesystemAddNewFileSystemEntry(&flashConfig, blinkSlowLength, 1);
}

void downloadConfigurationUSB(bool useFast) {
    uint8_t numberOfRequiredSectors;
    if (useFast) {
        numberOfRequiredSectors =
            (uint8_t)ceilf((float)blinkFastLength / (float)FLASH_BYTES_PER_SECTOR);
    } else {
        numberOfRequiredSectors =
            (uint8_t)ceilf((float)blinkSlowLength / (float)FLASH_BYTES_PER_SECTOR);
    }

    if (!filesystemFindFittingStartSector(numberOfRequiredSectors)) {
        PRINT("File does not fit. Check flash usage.\n");
        return;
    }

    PRINT("Please exit Minicom and start /bitfile_scripts/BitfileFlasher.py with the file path "
           "you selected \n (U = blink_fast / u = blink_slow)\n");
    while (stdio_usb_connected()) {}

    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaUsb(&flashConfig, nextFileSector);
    while (!stdio_usb_connected()) {}
    sleep_for_ms(10000);
    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Error 0x%02X occurred during download.", error);
        return;
    }
    PRINT("Download Successful!\n");

    if (useFast) {
        filesystemAddNewFileSystemEntry(&flashConfig, blinkFastLength, 1);
    } else {
        filesystemAddNewFileSystemEntry(&flashConfig, blinkSlowLength, 1);
    }
}

void readConfiguration(uint32_t sector) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;

    startAddress = sector * FLASH_BYTES_PER_SECTOR;

    numberOfPages = FLASH_BYTES_PER_SECTOR / FLASH_BYTES_PER_PAGE;

    numberOfPages = 5;

    PRINT("Sector: %d\n", sector);
    PRINT("Start Address: %d\n", startAddress);
    PRINT("\n");
    PRINT("\n");

    data_t pageBuffer = {.data = NULL, .length = FLASH_BYTES_PER_PAGE};
    while (page < numberOfPages) {
        pageBuffer.data = calloc(FLASH_BYTES_PER_PAGE, sizeof(uint8_t));
        flashReadData(&flashConfig, startAddress + (page * FLASH_BYTES_PER_PAGE), &pageBuffer);
        PRINT("Address: 0x%06lX", startAddress + (page * FLASH_BYTES_PER_PAGE));
        PRINT_BYTE_ARRAY("Configuration: ", pageBuffer.data, pageBuffer.length);
        free(pageBuffer.data);
        page++;
    }
}

uint32_t getInput() {
    char mystring[4] = {0};

    for (int i = 0; i < 4; i++) {
        char c = getchar_timeout_us(UINT32_MAX);
        if (c == 13) {
            break;
        }
        mystring[i] = c;
    }
    mystring[3] = '\0';
    uint32_t sector = atoi(mystring);
    return sector;
}

void printHelp() {
    PRINT("\n");
    PRINT("---------------------------------------------\n");
    PRINT("You can use the following commands:\n");
    PRINT("\n");
    PRINT("O or o: Output current File System sorted by Sector / ID\n");
    PRINT("f: Print current flash usage and number of free sectors\n");
    PRINT("m: Move file to a different sector\n");
    PRINT("E: Erase entire flash (use with caution)\n");
    PRINT("e: Erase file by ID\n");
    PRINT("D or d: Download fast or slow blinking config via HTTP\n");
    PRINT("U or u: Download fast or slow blinking config via USB\n");
    PRINT("P or p: Power FPGA on or off\n");
    PRINT("r: read data of sector\n");
    PRINT("---------------------------------------------\n");
    PRINT("\n");
}

_Noreturn void fileSystemDemo(void) {
    PRINT("Enter 'h' for help.\n");

    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'E':
            flashEraseAll(&flashConfig);
            PRINT("ERASED!\n");
            break;
        case 'e':
            PRINT("Which File do you want to erase? Enter ID: \n");
            uint8_t erase_id = getInput();
            if (filesystemEraseFileByID(&flashConfig, erase_id)) {
                PRINT("Erased File with ID %d.\n", erase_id);
            }
            break;

        case 'P':
            env5HwControllerFpgaPowersOn();
            PRINT("FPGA Power: ON\n");
            break;
        case 'p':
            env5HwControllerFpgaPowersOff();
            PRINT("FPGA Power: OFF\n");
            break;

        case 'U':
            downloadConfigurationUSB(true);
            break;
        case 'u':
            downloadConfigurationUSB(false);
            break;
        case 'D':
            downloadConfigurationHTTP(true);
            break;
        case 'd':
            downloadConfigurationHTTP(false);
            break;

        case 'r':
            PRINT("Which Sector do you want to read?\n");
            uint32_t read_sector = getInput();
            readConfiguration(read_sector);
            break;
        case 'm':
            PRINT("Which File do you want to move? Enter ID: \n");
            uint32_t ID = getInput();

            PRINT("Enter new Sector: \n");
            uint32_t newSector = getInput();
            filesystemMoveFileToSector(&flashConfig, ID, newSector);
            break;
        case 'f':
            PRINT("Number of free Sectors: %d\n", filesystemGetNumberOfFreeSectors());
            for (int i = 0; i < sizeof(sectorFree); i++) {
                PRINT("%d", sectorFree[i]);
                if (i == 5) {
                    PRINT(" | ");
                }
            }
            PRINT("\n");
            PRINT("\n");
            break;
        case 'O':
            filesystemSortFileSystemByStartSector();
            filesystemPrintFileSystem();
            break;
        case 'o':
            filesystemSortFileSystemByID();
            filesystemPrintFileSystem();
            break;
        case 'h':
            printHelp();
            break;
        default:
            PRINT("Enter 'h' for help.\n");
        }
    }
}

int main() {
    initDemo();
    filesystemInit(flashConfig);
    fileSystemDemo();
}
