#include "FpgaConfiguration.h"
#include "dummies/flash/Flash.h"
#include "dummies/flash/FpgaConfigDatahandler.h"
#include <stdbool.h>
#include <unity.h>

uint8_t expectedData[256 * 4];

void setUp() {
    for (uint16_t i = 0; i < 4; i++) {
        for (uint16_t j = 0; j < 256; j++) {
            expectedData[(j + i * 256)] = j;
        }
    }
}

void tearDown() {}

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
void testWriteDataToFlashAtAddress0() {
    uint32_t expectedAddresses[4];
    for (uint16_t i = 0; i < 4; i++) {
        expectedAddresses[i] = (256 * i);
    }
    fpgaConfigHandlerSetAddress(0);
    fpgaConfigHandlerSetConfigSize(256 * 4);
    fpgaConfigurationFlashConfiguration();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, addressBlockErase, "Number of Blocks erased");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedData, dataComplete, (256 * 4),
                                          "Content of written data");
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(addressWrite, expectedAddresses, 4,
                                           "Addresses of flash pages");
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testWriteDataToFlashAtAddress0);
    RUN_TEST(testReadValue);

    return UNITY_END();
}