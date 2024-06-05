#include "Flash.h"
#include "Common.h"
#include "FlashTypedefs.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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

int flashReadId(flashConfiguration_t *flashConfiguration, data_t *dataBuffer) {
    dataBuffer->data[0] = 0x01;
    dataBuffer->data[1] = 0x17;
    return 2;
}
int flashReadData(flashConfiguration_t *flashConfiguration, uint32_t startAddress, data_t *dataBuffer) {
    for (size_t offset = 0; offset < dataBuffer->length; offset++) {
        dataBuffer->data[offset] = flashStorage[startAddress + offset];
    }

    return (int)dataBuffer->length;
}

flashErrorCode_t flashEraseAll(flashConfiguration_t *flashConfiguration) {
    for (size_t index = 0; index < flashStorageLength; index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}
flashErrorCode_t flashEraseSector(flashConfiguration_t *flashConfiguration, uint32_t address) {
    for (size_t index = 0; index < address + (flashConfiguration->flashBytesPerSector); index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}
flashErrorCode_t flashErasePage(flashConfiguration_t *flashConfiguration, uint32_t address) {
    for (size_t index = 0; index < address + (flashConfiguration->flashBytesPerPage); index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}

int flashWritePage(flashConfiguration_t *flashConfiguration, uint32_t startAddress, uint8_t *data, size_t bytesToWrite) {
    for (size_t offset = 0; offset < bytesToWrite; offset++) {
        flashStorage[startAddress + offset] = data[offset];
    }

    return (int)bytesToWrite;
}
/* region PUBLIC HEADER FUNCTIONS */
