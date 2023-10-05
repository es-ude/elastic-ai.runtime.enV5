#include "Flash.h"
#include "FlashTypedefs.h"

#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

uint8_t *flashStorage;
size_t flashStorageLength;
bool flashInitCalled = false;

/* region UNIT TEST SETUP */
void flashSetUpDummyStorage(size_t lengthInBytes) {
    flashStorage = calloc(1, lengthInBytes);
    flashStorageLength = lengthInBytes;
}
void flashRemoveDummyStorage(void) {
    free(flashStorage);
    flashStorageLength = 0;
}
/* endregion UNIT TEST SETUP */

/* region PUBLIC HEADER FUNCTIONS */
void flashInit(spi_t *spiConfiguration, uint8_t chipSelectPin) {
    flashInitCalled = true;
}

int flashReadId(data_t *dataBuffer) {
    dataBuffer->data[0] = 0x01;
    dataBuffer->data[1] = 0x17;
    return 2;
}
int flashReadData(uint32_t startAddress, data_t *dataBuffer) {
    for (size_t offset = 0; offset < dataBuffer->length; offset++) {
        dataBuffer->data[offset] = flashStorage[startAddress + offset];
    }

    return (int)dataBuffer->length;
}

flashErrorCode_t flashEraseAll(void) {
    for (size_t index = 0; index < flashStorageLength; index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}
flashErrorCode_t flashEraseSector(uint32_t address) {
    for (size_t index = 0; index < address + FLASH_BYTES_PER_SECTOR; index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}
flashErrorCode_t flashErasePage(uint32_t address) {
    for (size_t index = 0; index < address + FLASH_BYTES_PER_PAGE; index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}

int flashWritePage(uint32_t startAddress, uint8_t *data, size_t bytesToWrite) {
    for (size_t offset = 0; offset < startAddress + bytesToWrite; offset++) {
        flashStorage[startAddress + offset] = data[offset];
    }

    return (int)bytesToWrite;
}
/* region PUBLIC HEADER FUNCTIONS */
