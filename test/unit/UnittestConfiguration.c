#include "FpgaConfiguration.h"
#include "dummies/flash/Flash.h"
#include "dummies/flash/FpgaConfigDatahandler.h"
#include <stdbool.h>
#include <unity.h>

const uint16_t blocksize = 256;
const uint16_t configSize = blocksize * 4;
uint8_t expectedData[65536 * 4];

void setUp() {
    numWriteBlocks = 0;
}

void tearDown() {
    for (uint16_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 65536; j++) {
            dataComplete[(j + i * blocksize)] = 0;
        }
    }
    addressSectorErase = 0;
    numSectorErase = 0;
}

uint16_t readData2(uint8_t *block, uint16_t bufferLength) {
    uint16_t bufferIndex = 0;
    while (true) {
        if (bufferIndex < bufferLength) {
            block[bufferIndex++] = 0xFF;
        } else {
            break;
        }
    }
    return bufferIndex;
}
void readValue2(uint32_t *destination) {
    readData2((uint8_t *)destination, sizeof(uint32_t));
}
void testReadValue() {
    uint32_t result;
    readValue2(&result);
    TEST_ASSERT_EQUAL((uint32_t)0xFFFFFFFF, result);
}

void writeExpectedData(uint32_t offset) {
    for (uint16_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 256; j++) {
            expectedData[(j + i * blocksize) + offset] = j;
        }
    }
}

void testWriteDataToFlashAtAddress0() {
    uint32_t expectedAddresses[4];
    writeExpectedData(0);

    for (uint16_t i = 0; i < 4; i++) {
        expectedAddresses[i] = (256 * i);
    }
    fpgaConfigHandlerSetAddress(0);
    fpgaConfigHandlerSetConfigSize(configSize);
    fpgaConfigurationFlashConfiguration();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, numSectorErase, "Number of Sectors erased");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, addressSectorErase, "Address of last Block erased");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedData, dataComplete, configSize,
                                          "Content of written data");
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expectedAddresses, addressWrite, 4,
                                           "Addresses of flash pages");
}

void testWriteDataToFlashAddress0x10000(){
    uint32_t expectedAddresses[4];
    writeExpectedData((uint32_t) 0x10000);

    for (uint16_t i = 0; i < 4; i++) {
        expectedAddresses[i] = ((uint32_t)0x10000)+(256 * i);
    }
    fpgaConfigHandlerSetAddress(((uint32_t)0x10000));
    fpgaConfigHandlerSetConfigSize(configSize);
    fpgaConfigurationFlashConfiguration();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, numSectorErase, "Number of Sectors erased");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t )0x10000, addressSectorErase, "Address of last Block erased");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((&expectedData[0] + 65536), (&dataComplete[0] + 65536), configSize,
                                          "Content of written data");
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expectedAddresses, addressWrite, 4,
                                           "Addresses of flash pages");
}

void testVerifyDataAtAddress0(){

}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testWriteDataToFlashAtAddress0);
    RUN_TEST(testWriteDataToFlashAddress0x10000);
    RUN_TEST(testReadValue);

    return UNITY_END();
}