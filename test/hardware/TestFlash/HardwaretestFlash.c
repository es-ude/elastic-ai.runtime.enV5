#define SOURCE_FILE "FLASH-HWTEST"

#include <malloc.h>
#include <math.h>
#include <stdio.h>

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "Spi.h"

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

static const uint32_t startAddress = 0x00000000;
const uint32_t pageLimit = 5;

void initializeConsoleOutput(void) {
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial console to connect
    }
}

void initializeHardware(void) {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();

    spiInit(&spiToFlashConfig);
}
void readDeviceID() {
    data_t idBuffer = {.data = calloc(3, sizeof(uint8_t)), .length = 3};
    int bytesRead = flashReadConfig(&flashConfig, FLASH_READ_ID, &idBuffer);
    PRINT_DEBUG("Bytes read: %i", bytesRead);
    PRINT("Device ID is: 0x%06X (Should be 0x010219)", *idBuffer.data);
    free(idBuffer.data);
}
void writeToFlash() {
    // create test double data
    uint8_t data[FLASH_BYTES_PER_PAGE];
    for (size_t index = 0; index < FLASH_BYTES_PER_PAGE; index++) {
        data[index] = (uint8_t)index;
    }
    data[1] = 0xFF;

    // write test double data to flash
    uint8_t pageCounter = 0;
    for (size_t pageOffset = 0; pageOffset < pageLimit * FLASH_BYTES_PER_PAGE;
         pageOffset += FLASH_BYTES_PER_PAGE) {
        data[0] = pageCounter;
        pageCounter++;
        int successfulWrittenBytes =
            flashWritePage(&flashConfig, startAddress + pageOffset, data, FLASH_BYTES_PER_PAGE);
        PRINT("Address 0x%02lX: %i Bytes Written", startAddress + pageOffset,
              successfulWrittenBytes);
    }
}
void readFromFlash() {
    for (size_t pageOffset = 0; pageOffset < pageLimit * FLASH_BYTES_PER_PAGE;
         pageOffset += FLASH_BYTES_PER_PAGE) {
        uint8_t data_read[FLASH_BYTES_PER_PAGE];
        data_t readBuffer = {.data = data_read, .length = FLASH_BYTES_PER_PAGE};

        int bytesRead = flashReadData(&flashConfig, startAddress + pageOffset, &readBuffer);
        PRINT("Address 0x%06lX: %u Bytes read", startAddress + pageOffset, bytesRead);
        PRINT_BYTE_ARRAY("Data: ", readBuffer.data, readBuffer.length);
    }
}
void eraseSectorFromFlash() {
    int sectorsToErase =
        (int)ceilf((float)(pageLimit * FLASH_BYTES_PER_PAGE) / FLASH_BYTES_PER_SECTOR);

    for (size_t sectorOffset = 0; sectorOffset < sectorsToErase * FLASH_BYTES_PER_SECTOR;
         sectorOffset += FLASH_BYTES_PER_SECTOR) {
        flashErrorCode_t eraseError =
            flashEraseSector(&flashConfig, FLASH_BYTES_PER_SECTOR * sectorOffset);
        PRINT("Sector starting from Address %lu erased. (0x%02X)", startAddress + sectorOffset,
              eraseError);

        // create read buffer
        uint8_t dataRead[FLASH_BYTES_PER_PAGE];
        data_t readBuffer = {.data = dataRead, .length = FLASH_BYTES_PER_PAGE};
        for (size_t pageOffset = 0; pageOffset < pageLimit * FLASH_BYTES_PER_PAGE;
             pageOffset += FLASH_BYTES_PER_PAGE) {
            flashReadData(NULL, startAddress + pageOffset, &readBuffer);
            for (size_t byteIndex = 0; byteIndex < FLASH_BYTES_PER_PAGE; byteIndex++) {
                if (dataRead[byteIndex] != 0xFF) {
                    PRINT("Erase Failed");
                    return;
                }
            }
        }
    }
    PRINT("Erase Successful");
}

_Noreturn void runTest(void) {
    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 'i':
            readDeviceID();
            break;
        case 'e':
            eraseSectorFromFlash();
            break;
        case 'w':
            writeToFlash();
            break;
        case 'r':
            readFromFlash();
            break;
        case 'b':
            flashEraseAll(NULL);
            break;
        default:
            PRINT("Waiting ...");
        }
    }
}

int main() {
    initializeConsoleOutput();
    initializeHardware();
    runTest();
}
