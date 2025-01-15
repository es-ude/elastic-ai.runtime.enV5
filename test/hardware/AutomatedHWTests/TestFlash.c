#define SOURCE_FILE "FLASH-HWTEST"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "Spi.h"

#include "pico/stdlib.h"
#include "unity.h"

#include "hardware/spi.h"
#include <math.h>
#include <stdlib.h>
#include <pico/bootrom.h>
#include <sys/unistd.h>

#define paramTest(fn, param)                                                                       \
    void fn##param(void) {                                                                         \
        fn(param);                                                                                 \
}

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};
flashConfiguration_t flashConfig = {.spiConfiguration = &spiToFlashConfig};

static uint16_t sector = 1;

void init(void) {
    stdio_init_all();
    while(!stdio_usb_connected()){}
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();
    spiInit(&spiToFlashConfig);
    flashInit(&flashConfig);

}

void readCorrectDeviceID() {
    uint8_t idData[3];
    data_t idBuffer = {.data = idData, .length = sizeof(idData)};
    int bytesRead = flashReadConfig(&flashConfig, FLASH_READ_ID, &idBuffer);
    // Manufacturer
    TEST_ASSERT_EQUAL(0x01, idBuffer.data[0]);
    // 0x 02 19 = 256MB | 0x 20 18 = 126MB
    TEST_ASSERT_EQUAL(0x02, idBuffer.data[1]);
    TEST_ASSERT_EQUAL(0x19, idBuffer.data[2]);
}

void writeToFlash() {
    // create test data
    uint8_t data[flashConfig.bytesPerPage];
    for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
        data[index] = (uint8_t)index;
    }
    bool correctNumberOfWrittenBytes = false;
    uint32_t sectorAddress = sector * flashConfig.bytesPerSector;

    // clear sector before writing (mandatory)
    flashEraseSector(&flashConfig, sectorAddress);
    // write test data to flash
    uint32_t numberOfPages = flashConfig.bytesPerSector / flashConfig.bytesPerPage;
    for (size_t pageIndex = 0; pageIndex < numberOfPages; pageIndex++) {
        int successfulWrittenBytes =
            flashWritePage(&flashConfig, sectorAddress + pageIndex * flashConfig.bytesPerPage, data, flashConfig.bytesPerPage);
        PRINT("Address 0x%02lX: %i Bytes Written", sectorAddress + pageIndex,
              successfulWrittenBytes);
        if (successfulWrittenBytes == flashConfig.bytesPerPage) {
            correctNumberOfWrittenBytes = true;
        }
    }

    TEST_ASSERT_EQUAL(correctNumberOfWrittenBytes, true);
}

void readFromFlash() {
    uint32_t sectorAddress = sector * flashConfig.bytesPerSector;
    uint32_t numberOfPages = flashConfig.bytesPerSector / flashConfig.bytesPerPage;
    bool dataCorrect = true;
    flashErrorCode_t error;

    uint32_t dataIndex = 0;

    for (size_t pageIndex = 0; pageIndex < numberOfPages; pageIndex++) {
        uint8_t data_read[flashConfig.bytesPerPage];
        data_t readBuffer = {.data = data_read, .length = flashConfig.bytesPerPage};

        error = flashReadData(&flashConfig, sectorAddress + (pageIndex * flashConfig.bytesPerPage), &readBuffer);
    }
    TEST_ASSERT_EQUAL(FLASH_NO_ERROR, error);
}

void checkIfReadbackMatchesOriginalData() {
    bool readbackCorrect = true;
    // create test data
    uint8_t testData[flashConfig.bytesPerPage];
    for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
        testData[index] = (uint8_t)index;
    }

    uint32_t sectorAddress = sector * flashConfig.bytesPerSector;
    uint32_t numberOfPages = flashConfig.bytesPerSector / flashConfig.bytesPerPage;

    for (size_t pageIndex = 0; pageIndex < numberOfPages; pageIndex++) {
        uint8_t data_read[flashConfig.bytesPerPage];
        data_t readBuffer = {.data = data_read, .length = flashConfig.bytesPerPage};

        flashReadData(&flashConfig, sectorAddress + (pageIndex * flashConfig.bytesPerPage), &readBuffer);
        for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
            if (testData[index] != readBuffer.data[index]) {
                readbackCorrect = false;
                PRINT_DEBUG("EXPECTED: %i, BUT WAS: %i", testData[index], readBuffer.data[index]);
            }
        }
    }
    TEST_ASSERT_EQUAL(true, readbackCorrect);
}

void checkIfReadbackFailsWithManipulatedData() {
    bool readbackCorrect = true;
    // create test data
    uint8_t testData[flashConfig.bytesPerPage];
    for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
        testData[index] = (uint8_t)index;
    }
    // manipulate testData
    testData[0] = 2;

    uint32_t sectorAddress = sector * flashConfig.bytesPerSector;
    uint32_t numberOfPages = flashConfig.bytesPerSector / flashConfig.bytesPerPage;

    for (size_t pageIndex = 0; pageIndex < numberOfPages; pageIndex++) {
        uint8_t data_read[flashConfig.bytesPerPage];
        data_t readBuffer = {.data = data_read, .length = flashConfig.bytesPerPage};

        flashReadData(&flashConfig, sectorAddress + (pageIndex * flashConfig.bytesPerPage), &readBuffer);
        for (size_t index = 0; index < flashConfig.bytesPerPage; index++) {
            if (testData[index] != readBuffer.data[index]) {
                readbackCorrect = false;
                PRINT_DEBUG("EXPECTED: %i, BUT WAS: %i", testData[index], readBuffer.data[index]);
                return;
            }
        }
    }
    TEST_ASSERT_EQUAL(false, readbackCorrect);
}

void eraseSectorFromFlash() {
    bool eraseFailed = false;
    bool sectorNotEmpty = false;
    uint32_t sectorAddress = sector * flashConfig.bytesPerSector;


    flashErrorCode_t eraseError =
        flashEraseSector(&flashConfig, sector * flashConfig.bytesPerSector);
    if (eraseError != FLASH_NO_ERROR) {
        eraseFailed = true;
    }

    // check if sector is actually empty
    uint8_t dataRead[flashConfig.bytesPerPage];
    data_t readBuffer = {.data = dataRead, .length = flashConfig.bytesPerPage};

    uint32_t numberOfPages = flashConfig.bytesPerSector / flashConfig.bytesPerPage;

    for (size_t pageIndex = 0; pageIndex < numberOfPages; pageIndex++) {
        flashReadData(&flashConfig, sectorAddress + (pageIndex * flashConfig.bytesPerPage), &readBuffer);
        for (size_t byteIndex = 0; byteIndex < flashConfig.bytesPerPage; byteIndex++) {
            if (dataRead[byteIndex] != 0xFF) {
                sectorNotEmpty = true;
                return;
            }
        }
    }
    TEST_ASSERT_EQUAL(false, eraseFailed);
    TEST_ASSERT_EQUAL(false, sectorNotEmpty);
}

void setUp(){}
void tearDown(){};

void deInit() {
    rom_reset_usb_boot(0, 0);
}

int main() {
    init();
    UNITY_BEGIN();
    RUN_TEST(readCorrectDeviceID);
    RUN_TEST(writeToFlash);
    RUN_TEST(readFromFlash);
    RUN_TEST(checkIfReadbackMatchesOriginalData);
    RUN_TEST(checkIfReadbackFailsWithManipulatedData);
    RUN_TEST(eraseSectorFromFlash);
    UNITY_END();
    deInit();
}
