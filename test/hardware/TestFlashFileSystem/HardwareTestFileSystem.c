#define SOURCE_FILE "DEMO_FILESYSTEM"

#include <stdlib.h>

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include "eai/Common.h"
#include "eai/flash/FileSystem.h"
#include "eai/flash/Flash.h"
#include "eai/fpga/FpgaConfigurationHandler.h"
#include "eai/hal/EnV5HwConfiguration.h"
#include "eai/hal/EnV5HwController.h"
#include "eai/hal/Sleep.h"
#include "eai/network/Esp.h"
#include "eai/network/Network.h"

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};

flashConfiguration_t flashConfig = {
    .spiConfiguration = &spiToFlashConfig,
};

filesystemConfiguration_t filesystemConfiguration;

// This needs to be filled with the URL given by the HW-Test_Webserver.py script
const char *baseUrl = "";

size_t blinkFastLength = 86116;
size_t blinkSlowLength = 85540;

void initDemo() {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    stdio_init_all();
    while (!stdio_usb_connected()) {}

    espInit();
    networkTryToConnectToNetworkUntilSuccessful();

    flashInit(&flashConfig);
    filesystemInit(&flashConfig, &filesystemConfiguration);

    sleep_for_ms(1000);
    PRINT("\n");
    PRINT("===== File System Demo =====\n");
}

void downloadConfigurationHTTP(bool useFast) {
    fpgaConfigurationHandlerError_t error;
    char url[125];
    strcpy(url, baseUrl);

    if (useFast) {
        int32_t nextFileSector =
            filesystemFindFittingStartSector(&filesystemConfiguration, blinkFastLength);
        if (nextFileSector < 0) {
            return;
        }

        strcat(url, "/getfast");
        PRINT_DEBUG("URL: %s", url);
        error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
            &flashConfig, url, blinkFastLength, nextFileSector);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }
        filesystemAddNewFileSystemEntry(&filesystemConfiguration, nextFileSector, blinkFastLength,
                                        1);
    }

    else {
        int32_t nextFileSector =
            filesystemFindFittingStartSector(&filesystemConfiguration, blinkSlowLength);
        if (nextFileSector < 0) {
            PRINT("Not enough space...\n Aborting...\n");
            return;
        }

        strcat(url, "/getslow");
        PRINT_DEBUG("URL: %s", url);
        error = fpgaConfigurationHandlerDownloadConfigurationViaHttp(
            &flashConfig, url, blinkSlowLength, nextFileSector);
        if (error != FPGA_RECONFIG_NO_ERROR) {
            PRINT("Error 0x%02X occurred during download.", error);
            return;
        }
        filesystemAddNewFileSystemEntry(&filesystemConfiguration, nextFileSector, blinkSlowLength,
                                        1);
    }
    PRINT("Download Successful!");
}

void downloadConfigurationUSB(bool useFast) {

    uint32_t numberOfRequiredBytes = useFast ? blinkFastLength : blinkSlowLength;
    int32_t startSector =
        filesystemFindFittingStartSector(&filesystemConfiguration, numberOfRequiredBytes);

    if (startSector < 0) {
        PRINT("File does not fit. Check flash usage.\n");
        return;
    }

    PRINT("Please exit Minicom and start /bitfile_scripts/BitfileFlasher.py with the file path "
          "you selected \n (U = blink_fast / u = blink_slow)\n");
    while (stdio_usb_connected()) {}

    fpgaConfigurationHandlerError_t error =
        fpgaConfigurationHandlerDownloadConfigurationViaUsb(&flashConfig, startSector);

    while (!stdio_usb_connected()) {}
    sleep_for_ms(10000);

    if (error != FPGA_RECONFIG_NO_ERROR) {
        PRINT("Error 0x%02X occurred during download.", error);
        return;
    }
    PRINT("Download Successful!\n");

    if (useFast) {
        filesystemAddNewFileSystemEntry(&filesystemConfiguration, startSector, blinkFastLength, 1);
    } else {
        filesystemAddNewFileSystemEntry(&filesystemConfiguration, startSector, blinkSlowLength, 1);
    }
}

void readConfiguration(uint32_t sector) {
    size_t numberOfPages, page = 0;
    uint32_t startAddress;

    startAddress = sector * flashGetBytesPerSector(&flashConfig);

    numberOfPages = flashGetBytesPerSector(&flashConfig) / flashGetBytesPerPage(&flashConfig);

    numberOfPages = 5;

    PRINT("Sector: %lu\n", sector);
    PRINT("Start Address: %lu\n", startAddress);
    PRINT("\n");
    PRINT("\n");

    data_t pageBuffer = {.data = NULL, .length = flashGetBytesPerPage(NULL)};
    while (page < numberOfPages) {
        pageBuffer.data = calloc(flashGetBytesPerPage(NULL), sizeof(uint8_t));
        flashReadData(&flashConfig, startAddress + (page * flashGetBytesPerPage(NULL)),
                      &pageBuffer);
        PRINT("Address: 0x%06lX", startAddress + (page * flashGetBytesPerPage(NULL)));
        PRINT_BYTE_ARRAY("Configuration: ", pageBuffer.data, pageBuffer.length);
        free(pageBuffer.data);
        page++;
    }
}

uint32_t getInput() {
    char mystring[20] = {0};

    for (int i = 0; i < 20; i++) {
        char c = getchar_timeout_us(UINT32_MAX);
        if (c == 13) {
            break;
        }
        mystring[i] = c;
    }
    mystring[19] = '\0';
    uint32_t sector = strtol(mystring, NULL, 10);
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
    PRINT("b: Block required bytes for FPGA access only\n");
    PRINT("F: Free all blocked sectors\n");
    PRINT("c: Clear all entries in the filesystem\n");
    PRINT("---------------------------------------------\n");
    PRINT("\n");
}

void printFileSystemEntry(fileSystemEntry_t entry) {
    PRINT("ID:                %d\n", entry.entry.id);
    PRINT("Start Sector:      %d\n", entry.entry.startSector);
    PRINT("Size:              %lu\n", entry.entry.size);
    PRINT("isConfig:          %d\n", entry.entry.isConfig);
    PRINT("Number of Sectors: %d\n", entry.entry.numberOfSectors);
    PRINT("---\n");
}

void filesystemPrintFileSystem(filesystemConfiguration_t *filesystemConfig) {
    PRINT("\n");
    PRINT("---File System---\n");
    PRINT("Number of entries: %i\n", filesystemConfig->numberOfEntries);
    PRINT("\n");
    for (int i = 0; i < filesystemConfig->numberOfEntries; i++) {
        printFileSystemEntry(filesystemConfig->fileSystem[i]);
    }
    PRINT_DEBUG("\n");
}

_Noreturn void fileSystemDemo() {
    PRINT("Enter 'h' for help.\n");

    while (true) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'b':

            PRINT("Enter start sector where the protected section should start: \n");
            uint32_t startSector = getInput();

            PRINT("Enter the number of required bytes: \n");
            uint32_t numberOfBytes = getInput();

            if (filesystemBlockBytesForFPGA(&filesystemConfiguration, startSector, numberOfBytes)) {
                PRINT("Sectors blocked!\n");
            } else {
                PRINT("Sector already contains data. Please check the filesystem and choose a "
                      "different sector.");
            }

            break;
        case 'c':
            filesystemEraseAllEntries(&filesystemConfiguration);
            PRINT("Filesystem erased!\n");
            break;
        case 'D':
            downloadConfigurationHTTP(true);
            break;
        case 'd':
            downloadConfigurationHTTP(false);
            break;
        case 'E':
            flashEraseAll(&flashConfig);
            PRINT("ERASED!\n");
            break;
        case 'e':
            PRINT("Which File do you want to erase? Enter ID: \n");
            uint8_t erase_id = getInput();
            if (filesystemEraseFileByID(&filesystemConfiguration, erase_id)) {
                PRINT("Erased File with ID %d.\n", erase_id);
            }
            break;
        case 'f':
            PRINT("Number of free Sectors: %d\n",
                  filesystemGetNumberOfFreeSectors(&filesystemConfiguration));
            for (int i = 0; i < sizeof(filesystemConfiguration.sectorFree); i++) {
                PRINT("Sector %i: %d", i, filesystemConfiguration.sectorFree[i]);
                if (i == 1018) {
                    PRINT("--");
                }
            }
            break;
        case 'F':
            filesystemFreeBlockedFPGASectors(&filesystemConfiguration);
            PRINT("Cleared all blocked sectors!");
            break;
        case 'h':
            printHelp();
            break;
        case 'm':
            PRINT("Which File do you want to move? Enter ID: \n");
            uint32_t ID = getInput();

            PRINT("Enter new Sector: \n");
            uint32_t newSector = getInput();
            if (filesystemMoveFileToSector(&filesystemConfiguration, ID, newSector)) {
                PRINT("Moving file successful!");
            } else {
                PRINT("Moving of file failed. Check debug output for more information.");
            }
            break;
        case 'O':
            filesystemSortFileSystemByStartSector();
            filesystemPrintFileSystem(&filesystemConfiguration);
            break;
        case 'o':
            filesystemSortFileSystemByID();
            filesystemPrintFileSystem(&filesystemConfiguration);
            break;
        case 'P':
            env5HwControllerFpgaPowersOn();
            PRINT("FPGA Power: ON\n");
            break;
        case 'p':
            env5HwControllerFpgaPowersOff();
            PRINT("FPGA Power: OFF\n");
            break;
        case 'r':
            PRINT("Which Sector do you want to read?\n");
            uint32_t read_sector = getInput();
            readConfiguration(read_sector);
            break;
        case 'U':
            downloadConfigurationUSB(true);
            break;
        case 'u':
            downloadConfigurationUSB(false);
            break;
        case 'z':
            printf("%i\n", filesystemConfiguration.filesystemStartSector);
            printf("%i\n", filesystemConfiguration.filesystemEndSector);
            printf("%lu\n", filesystemConfiguration.nextFileSystemSector);
            printf("\n");
            printf("%i\n", filesystemConfiguration.numberOfEntries);
            printf("%i\n", filesystemConfiguration.fileID);
            break;
        default:
            PRINT("Enter 'h' for help.\n");
        }
    }
}

int main() {
    initDemo();
    fileSystemDemo();
}
