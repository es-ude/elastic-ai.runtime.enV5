/*! Datasheet:
 *    https://www.mouser.de/datasheet/2/196/Infineon_S25FL128SS25FL256S_128_Mb__16_MB_256_Mb__-3166946.pdf
 */

#define SOURCE_FILE "FLASH"

#include <stdint.h>

#include "Common.h"
#include "FlashInternal.h"
#include "Sleep.h"
#include "Spi.h"
#include "SpiTypedefs.h"
#include "include/Flash.h"
#include "include/FlashTypedefs.h"

static spi_t *flashSpi;
static uint8_t flashChipSelectPin;

/* region PUBLIC HEADER FUNCTIONS */

void flashInit(spi_t *spiConfiguration, uint8_t chipSelectPin) {
    flashChipSelectPin = chipSelectPin;
    flashSpi = spiConfiguration;
}

int flashReadId(data_t *dataBuffer) {
    uint8_t cmd[1] = {FLASH_READ_ID};
    data_t command = {.data = cmd, .length = 1};

    spiInit(flashSpi, flashChipSelectPin);
    int readBytes =
        spiWriteCommandAndReadBlocking(flashSpi, flashChipSelectPin, &command, dataBuffer);
    spiDeinit(flashSpi, flashChipSelectPin);
    return readBytes;
}
int flashReadData(uint32_t startAddress, data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpi, flashChipSelectPin);
    int bytesRead =
        spiWriteCommandAndReadBlocking(flashSpi, flashChipSelectPin, &command, dataBuffer);
    spiDeinit(flashSpi, flashChipSelectPin);
    return bytesRead;
}

flashErrorCode_t flashEraseAll(void) {
    uint8_t cmd[] = {FLASH_BULK_ERASE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpi, flashChipSelectPin);
    flashEnableWrite();

    spiWriteCommandBlocking(flashSpi, flashChipSelectPin, &command);
    sleep_for_ms(33000);
    flashWaitForDone();
    flashErrorCode_t eraseError = flashEraseErrorOccurred();
    spiDeinit(flashSpi, flashChipSelectPin);

    return eraseError;
}
flashErrorCode_t flashEraseSector(uint32_t address) {
    uint8_t cmd[] = {FLASH_ERASE_SECTOR, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpi, flashChipSelectPin);
    flashEnableWrite();

    spiWriteCommandBlocking(flashSpi, flashChipSelectPin, &command);

    flashWaitForDone();
    flashErrorCode_t eraseError = flashEraseErrorOccurred();
    spiDeinit(flashSpi, flashChipSelectPin);

    return eraseError;
}
flashErrorCode_t flashErasePage(uint32_t address) {
    // prepare data for override (set all bits to 1)
    uint8_t data[FLASH_BYTES_PER_PAGE];
    for (size_t index = 0; index < FLASH_BYTES_PER_PAGE; index++) {
        data[index] = 0xFF;
    }

    // override old data
    flashWritePage(address, data, FLASH_BYTES_PER_PAGE);
}

int flashWritePage(uint32_t startAddress, uint8_t *data, size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(flashSpi, flashChipSelectPin);
    flashEnableWrite();

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashSpi, flashChipSelectPin, &command, &dataToWrite);

    flashWaitForDone();
    spiDeinit(flashSpi, flashChipSelectPin);

    return bytesWritten;
}

/* endregion PUBLIC HEADER FUNCTIONS */

/* region INTERNAL HEADER FUNCTIONS */

static void flashEnableWrite(void) {
    uint8_t cmd[] = {FLASH_ENABLE_WRITE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiWriteCommandBlocking(flashSpi, flashChipSelectPin, &command);
}

static uint8_t flashReadStatusRegister(void) {
    uint8_t cmd[] = {FLASH_STATUS_REGISTER};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    uint8_t statusRegister[1];
    data_t statusRegisterData = {.data = statusRegister, .length = sizeof(statusRegister)};

    spiWriteCommandAndReadBlocking(flashSpi, flashChipSelectPin, &command, &statusRegisterData);
    PRINT_DEBUG("Status Register: 0x%02X", statusRegisterData.data[0])
    return statusRegisterData.data[0];
}

static flashErrorCode_t flashEraseErrorOccurred(void) {
    return ((flashReadStatusRegister() >> 5) & 1) ? FLASH_ERASE_ERROR : FLASH_NO_ERROR;
}

static void flashWaitForDone(void) {
    while (flashReadStatusRegister() & 0x01) {}
}
/* endregion INTERNAL HEADER FUNCTIONS */