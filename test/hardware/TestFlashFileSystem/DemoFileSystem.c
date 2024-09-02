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

#define SOURCE_FILE "CONFIGURE-HWTEST"

#include "stdlib.h"
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
                                       .csPin = FLASH_SPI_CS};
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
    printf("\n");
    printf("===== File System Demo =====\n");
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
        if (!findFittingStartSector(numberOfRequiredSectors)) {
            return;
        }
        addNewFileSystemEntry(&flashConfig, blinkFastLength, 1);
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
        if (!findFittingStartSector(numberOfRequiredSectors)) {
            printf("Not enough space...\n Aborting...\n");
            return;
        }
        addNewFileSystemEntry(&flashConfig, blinkSlowLength, 1);
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

    if (!findFittingStartSector(numberOfRequiredSectors)) {
        printf("File does not fit. Check flash usage.\n");
        return;
    }

    printf("Please exit Minicom and start /bitfile_scripts/BitfileFlasher.py with the file path "
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
    printf("Download Successful!\n");

    if (useFast) {
        addNewFileSystemEntry(&flashConfig, blinkFastLength, 1);
    } else {
        addNewFileSystemEntry(&flashConfig, blinkSlowLength, 1);
    }
}

void readConfiguration(uint32_t sector) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;

    startAddress = sector * FLASH_BYTES_PER_SECTOR;

    numberOfPages = FLASH_BYTES_PER_SECTOR / FLASH_BYTES_PER_PAGE;

    numberOfPages = 5;

    printf("Sector: %d\n", sector);
    printf("Start Address: %d\n", startAddress);
    printf("\n");
    printf("\n");

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
    printf("\n");
    printf("---------------------------------------------\n");
    printf("You can use the following commands:\n");
    printf("\n");
    printf("O or o: Output current File System sorted by Sector / ID\n");
    printf("f: Print current flash usage and number of free sectors\n");
    printf("m: Move file to a different sector\n");
    printf("E: Erase entire flash (use with caution)\n");
    printf("e: Erase file by ID\n");
    printf("D or d: Download fast or slow blinking config via HTTP\n");
    printf("U or u: Download fast or slow blinking config via USB\n");
    printf("P or p: Power FPGA on or off\n");
    printf("r: read data of sector\n");
    printf("---------------------------------------------\n");
    printf("\n");
}

_Noreturn void fileSystemDemo(void) {
    printf("Enter 'h' for help.\n");

    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'E':
            flashEraseAll(&flashConfig);
            printf("ERASED!\n");
            break;
        case 'e':
            printf("Which File do you want to erase? Enter ID: \n");
            uint8_t erase_id = getInput();
            if (eraseFileByID(&flashConfig, erase_id)) {
                printf("Erased File with ID %d.\n", erase_id);
            }
            break;

        case 'P':
            env5HwControllerFpgaPowersOn();
            printf("FPGA Power: ON\n");
            break;
        case 'p':
            env5HwControllerFpgaPowersOff();
            printf("FPGA Power: OFF\n");
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
            printf("Which Sector do you want to read?\n");
            uint32_t read_sector = getInput();
            readConfiguration(read_sector);
            break;
        case 'm':
            printf("Which File do you want to move? Enter ID: \n");
            uint32_t ID = getInput();

            printf("Enter new Sector: \n");
            uint32_t newSector = getInput();
            moveFileToSector(&flashConfig, ID, newSector);
            break;
        case 'f':
            printf("Number of free Sectors: %d\n", getNumberOfFreeSectors());
            for (int i = 0; i < sizeof(sectorFree); i++) {
                printf("%d", sectorFree[i]);
                if (i == 5) {
                    printf(" | ");
                }
            }
            printf("\n");
            printf("\n");
            break;
        case 'O':
            sortFileSystemByStartSector();
            printFileSystem();
            break;
        case 'o':
            sortFileSystemByID();
            printFileSystem();
            break;
        case 'h':
            printHelp();
            break;
        default:
            printf("Enter 'h' for help.\n");
        }
    }
}

int main() {
    initDemo();
    initFileSystem(flashConfig);
    fileSystemDemo();
}
