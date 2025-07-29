#include "eai/flash/Flash.h"
// #include "eai/Common.h"
#include "eai/flash/FlashTypedefs.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t *flashStorage;
size_t flashStorageLength;

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

void flashInit(flashConfiguration_t *flashConfig) {
    // Just here to satisfy the compiler
}

size_t flashGetBytesPerSector(flashConfiguration_t *flashConfig) {
    return 262144; // == 256KiB
}
size_t flashGetBytesPerPage(flashConfiguration_t *flashConfig) {
    return 512; // == 512B
}
size_t flashGetNumberOfBytes(flashConfiguration_t *flashConfig) {
    return 134217728; // == 128MiB
}
size_t flashGetNumberOfSectors(flashConfiguration_t *flashConfig) {
    return (uint32_t)(flashGetNumberOfBytes(NULL) / flashGetBytesPerSector(NULL));
}

int flashReadId(flashConfiguration_t *flashConfiguration, data_t *dataBuffer) {
    dataBuffer->data[0] = 0x01;
    dataBuffer->data[1] = 0x17;
    return 2;
}
int flashReadData(flashConfiguration_t *flashConfiguration, uint32_t startAddress,
                  data_t *dataBuffer) {
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
    for (size_t index = 0; index < address + (flashConfiguration->bytesPerSector); index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}
flashErrorCode_t flashErasePage(flashConfiguration_t *flashConfiguration, uint32_t address) {
    for (size_t index = 0; index < address + (flashConfiguration->bytesPerPage); index++) {
        flashStorage[index] = 0xFF;
    }
    return FLASH_NO_ERROR;
}

int flashWritePage(flashConfiguration_t *flashConfiguration, uint32_t startAddress, uint8_t *data,
                   size_t bytesToWrite) {
    for (size_t offset = 0; offset < bytesToWrite; offset++) {
        flashStorage[startAddress + offset] = data[offset];
    }

    return (int)bytesToWrite;
}

/* region PUBLIC HEADER FUNCTIONS */
