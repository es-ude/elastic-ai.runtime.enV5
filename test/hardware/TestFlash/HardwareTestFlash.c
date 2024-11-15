#define SOURCE_FILE "FLASH-HWTEST"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"

#include "Flash.h"
#include "Spi.h"

#include "pico/stdlib.h"

#include "hardware/spi.h"
#include <math.h>
#include <stdlib.h>

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
    initializeFlashConfig();
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
    uint8_t data[flashConfig.bytesPerPage];
    for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
        data[index] = (uint8_t)index;
    }
    data[1] = 0xFF;

    // write test double data to flash
    uint8_t pageCounter = 0;
    for (size_t pageOffset = 0; pageOffset < pageLimit * flashConfig.bytesPerPage;
         pageOffset += flashConfig.bytesPerPage) {
        data[0] = pageCounter;
        pageCounter++;
        int successfulWrittenBytes =
            flashWritePage(&flashConfig, startAddress + pageOffset, data, flashConfig.bytesPerPage);
        PRINT("Address 0x%02lX: %i Bytes Written", startAddress + pageOffset,
              successfulWrittenBytes);
    }
}

void readFromFlash() {
    for (size_t pageOffset = 0; pageOffset < pageLimit * flashConfig.bytesPerPage;
         pageOffset += flashConfig.bytesPerPage) {
        uint8_t data_read[flashConfig.bytesPerPage];
        data_t readBuffer = {.data = data_read, .length = flashConfig.bytesPerPage};

        int bytesRead = flashReadData(&flashConfig, startAddress + pageOffset, &readBuffer);
        PRINT("Address 0x%06lX: %u Bytes read", startAddress + pageOffset, bytesRead);
        PRINT_BYTE_ARRAY("Data: ", readBuffer.data, readBuffer.length);
    }
}

void eraseSectorFromFlash() {
    int sectorsToErase = (int)ceilf((float)(pageLimit * flashConfig.bytesPerPage) /
                                    (float)flashConfig.bytesPerSector);

    for (size_t sectorOffset = 0; sectorOffset < sectorsToErase * flashConfig.bytesPerSector;
         sectorOffset += flashConfig.bytesPerSector) {
        flashErrorCode_t eraseError =
            flashEraseSector(&flashConfig, flashConfig.bytesPerSector * sectorOffset);
        PRINT("Sector starting from Address %lu erased. (0x%02X)", startAddress + sectorOffset,
              eraseError);

        // create read buffer
        uint8_t dataRead[flashConfig.bytesPerPage];
        data_t readBuffer = {.data = dataRead, .length = flashConfig.bytesPerPage};
        for (size_t pageOffset = 0; pageOffset < pageLimit * flashConfig.bytesPerPage;
             pageOffset += flashConfig.bytesPerPage) {
            flashReadData(NULL, startAddress + pageOffset, &readBuffer);
            for (size_t byteIndex = 0; byteIndex < flashConfig.bytesPerPage; byteIndex++) {
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
