#include "Flash.h"
#include "FlashTypedefs.h"
#include "FpgaConfigDatahandler.h"
#include "FpgaConfiguration.h"
#include <unity.h>

#define NUMSECTORS 2
const uint8_t numberOfPages = 5;
uint32_t config_size = FLASH_PAGE_SIZE * numberOfPages;
const uint8_t numberOfSectors = NUMSECTORS;
uint8_t expectedData[FLASH_SECTOR_SIZE * NUMSECTORS];

void setUp(void) {
    fpgaConfigHandlerNumWrittenBlock = 0;
    numWriteBlocks = 0;
    for (uint16_t i = 0; i < numberOfSectors; i++) {
        for (uint32_t j = 0; j < FLASH_SECTOR_SIZE; j++) {
            dataComplete[(j + i * FLASH_PAGE_SIZE)] = 0;
        }
    }
    addressSectorErase = 0;
    numSectorErase = 0;
}

void tearDown(void) {}

void writeExpectedData(uint32_t offset) {
    for (uint16_t i = 0; i < numberOfPages; i++) {
        for (uint32_t j = 0; j < FLASH_PAGE_SIZE; j++) {
            expectedData[(j + i * FLASH_PAGE_SIZE) + offset] = j;
        }
    }
}
void writeExpectedAddresses(uint32_t *expectedAddresses, uint32_t startingAddress) {
    for (uint16_t i = 0; i < numberOfPages; i++) {
        expectedAddresses[i] = startingAddress + (FLASH_PAGE_SIZE * i);
    }
}

void writeDataToFlash(uint32_t address) {
    uint32_t expectedAddresses[numberOfPages];
    writeExpectedData(address);
    writeExpectedAddresses(expectedAddresses, address);
    fpgaConfigHandlerSetAddress(address);
    fpgaConfigHandlerSetConfigSize(config_size);
    fpgaConfigurationFlashConfiguration();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, numSectorErase, "Number of Sectors erased");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(address, addressSectorErase, "Address of last Block erased");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((&expectedData[0] + address),
                                          (&dataComplete[0] + address), config_size,
                                          "Content of written data");
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expectedAddresses, addressWrite, numberOfPages,
                                           "Addresses of flash pages");
}
void testWriteDataToFlashAtAddress0() {
    writeDataToFlash((uint32_t)0x00);
}

void testWriteDataToFlashAddress0x10000() {
    writeDataToFlash((uint32_t)0x10000);
}

void verifyData(uint32_t address) {
    writeExpectedData(address);
    flashSetData(expectedData, FLASH_SECTOR_SIZE * numberOfSectors);
    fpgaConfigHandlerSetAddress(address);
    fpgaConfigHandlerSetConfigSize(config_size);
    fpgaConfigurationVerifyConfiguration();
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((&expectedData[0] + address), dataSent, config_size,
                                          "Verification of Data in Flash");
}
void testVerifyDataAtAddress0() {
    verifyData((uint32_t)0x000);
}
void testVerifyDataAtAddress0x10000() {
    verifyData((uint32_t)0x10000);
}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testWriteDataToFlashAtAddress0);
    RUN_TEST(testWriteDataToFlashAddress0x10000);
    //    RUN_TEST(testVerifyDataAtAddress0);
    RUN_TEST(testVerifyDataAtAddress0x10000);
    return UNITY_END();
}