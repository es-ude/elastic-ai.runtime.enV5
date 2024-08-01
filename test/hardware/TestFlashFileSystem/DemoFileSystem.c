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
#include "stdlib.h"
#include <string.h>

#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "Network.h"
#include "FileSystem.h"


spiConfiguration_t spiToFlashConfig = {.sckPin = SPI_FLASH_SCK,
                                       .misoPin = SPI_FLASH_MISO,
                                       .mosiPin = SPI_FLASH_MOSI,
                                       .baudrate = SPI_FLASH_BAUDRATE,
                                       .spiInstance = SPI_FLASH_INSTANCE,
                                       .csPin = SPI_FLASH_CS};
flashConfiguration_t flashConfig = {
    .flashSpiConfiguration = &spiToFlashConfig,
    .flashBytesPerPage = FLASH_BYTES_PER_PAGE,
    .flashBytesPerSector = FLASH_BYTES_PER_SECTOR,
};

/*const char *baseUrl = "http://192.168.2.46:5000";*/
const char *baseUrl = "http://192.168.203.223:5000";
uint8_t blinkFast = 40;
size_t blinkFastLength = 86116;
uint8_t blinkSlow =42;
size_t blinkSlowLength = 85540;

void initHardwareTest() {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    // initialize the serial output
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial connection
    }

    espInit(); // initilize Wi-Fi chip
    networkTryToConnectToNetworkUntilSuccessful();

    fpgaConfigurationHandlerInitialize();
}

void downloadConfiguration(bool useFast) {

    fpgaConfigurationHandlerError_t error;
    char url[125];
    strcpy(url, baseUrl);

    if (useFast) {
        strcat(url, "/getfast");
        PRINT_DEBUG("URL: %s", url);

        //check ob File noch passt

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

    /*if (useFast) {
        char name[] = "fast";
        addNewFileSystemEntry(name, blinkFast, blinkFastLength, 1);
        writeFileSystemToFlash(&flashConfig);
    } else {
        char name[] = "slow";
        addNewFileSystemEntry(name, blinkSlow, blinkSlowLength, 1);
        writeFileSystemToFlash(&flashConfig);
    }*/
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

uint32_t getSectorInput() {
    char mystring[4] = {0};

    for (int i = 0; i < 3; i++){
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

_Noreturn void configurationTest(void) {
    PRINT("===== START TEST =====");

    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'E':
            flashEraseAll(&flashConfig);
            PRINT("ERASED!");
            break;
        case 'e':
            PRINT("Which Sector do you want to erase?\n");
            uint32_t erase_sector = getSectorInput();

            uint32_t startAddress = erase_sector * (FLASH_BYTES_PER_SECTOR);
            flashEraseSector(&flashConfig, startAddress);
            printf("Erased Sector %d.\n", erase_sector);
            break;
        case 'P':
            env5HwControllerFpgaPowersOn();
            PRINT("FPGA Power: ON");
            break;
        case 'p':
            env5HwControllerFpgaPowersOff();
            PRINT("FPGA Power: OFF");
            break;
        case 'D':
            downloadConfiguration(false);
            break;
        case 'd':
            downloadConfiguration(true);
            break;
        case 'r':
            PRINT("Which Sector do you want to read?");
            uint32_t read_sector = getSectorInput();
            readConfiguration(read_sector);
            break;
        case 'm':
            PRINT("Which Sector do you want to move?");
            uint32_t currentSector = getSectorInput();

            PRINT("Enter new Sector: ");
            uint32_t newSector = getSectorInput();
            moveFileToSector(&flashConfig, currentSector, newSector);
            break;
        case 'f':
            printf("Number of free Sectors: %d\n", getNumberOfFreeSectors());
            for (int i = 0; i < sizeof(sectorFree); i++) {
                printf("%d", sectorFree[i]);
            }
            printf("\n");
            break;
        case 'o':
            printFileSystem();
            break;
        case 's':
            printFSS();
            break;
        case 'C':
            checkNumberOfEntries(&flashConfig);
            break;
        case 'i':
            inc();
            break;
        default:
            PRINT("Waiting ...");
        }
    }
}

int main() {
    initHardwareTest();
    initFileSystem(flashConfig);
    downloadConfiguration(true);
    readConfiguration(blinkFast);
    downloadConfiguration(false);
    readConfiguration(blinkSlow);
    readConfiguration(blinkFast);
    configurationTest();
}
