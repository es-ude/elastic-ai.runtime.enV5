/* Datasheet:
 *    https://www.mouser.de/datasheet/2/196/Infineon_S25FL128SS25FL256S_128_Mb__16_MB_256_Mb__-3166946.pdf
 *
 *  IMPORTANT:
 *    Because the QXSPI-library uses the same SPI module,
 *    before every operation the module has to be initialized!
 */

#define SOURCE_FILE "FLASH"

#include <stdint.h>

#include "Common.h"
#include "Flash.h"
#include "FlashInternal.h"
#include "FlashTypedefs.h"
#include "Sleep.h"
#include "Spi.h"
#include "SpiTypedefs.h"

/* region PUBLIC HEADER FUNCTIONS */
int flashReadId(flashConfiguration_t *flashConfiguration, data_t *dataBuffer) {
    uint8_t cmd[1] = {FLASH_READ_ID};
    data_t command = {.data = cmd, .length = 1};

    spiInit(flashConfiguration->flashSpiConfiguration);
    int readBytes = spiWriteCommandAndReadBlocking(flashConfiguration->flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(flashConfiguration->flashSpiConfiguration);
    return readBytes;
}
int flashReadData(flashConfiguration_t *flashConfiguration, uint32_t startAddress, data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfiguration->flashSpiConfiguration);
    int bytesRead = spiWriteCommandAndReadBlocking(flashConfiguration->flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(flashConfiguration->flashSpiConfiguration);
    return bytesRead;
}

flashErrorCode_t flashEraseAll(flashConfiguration_t *flashConfiguration) {
    uint8_t cmd[] = {FLASH_BULK_ERASE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfiguration->flashSpiConfiguration);
    flashEnableWrite(flashConfiguration->flashSpiConfiguration);

    spiWriteCommandBlocking(flashConfiguration->flashSpiConfiguration, &command);
    sleep_for_ms(33000);
    flashWaitForDone(flashConfiguration->flashSpiConfiguration);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(flashConfiguration->flashSpiConfiguration);
    spiDeinit(flashConfiguration->flashSpiConfiguration);

    return eraseError;
}
flashErrorCode_t flashEraseSector(flashConfiguration_t *flashConfiguration, uint32_t address) {
    uint8_t cmd[] = {FLASH_ERASE_SECTOR, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfiguration->flashSpiConfiguration);
    flashEnableWrite(flashConfiguration->flashSpiConfiguration);

    spiWriteCommandBlocking(flashConfiguration->flashSpiConfiguration, &command);

    flashWaitForDone(flashConfiguration->flashSpiConfiguration);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(flashConfiguration->flashSpiConfiguration);
    spiDeinit(flashConfiguration->flashSpiConfiguration);

    return eraseError;
}

int flashWritePage(flashConfiguration_t *flashConfiguration, uint32_t startAddress, uint8_t *data,
                   size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(flashConfiguration->flashSpiConfiguration);
    flashEnableWrite(flashConfiguration->flashSpiConfiguration);

    int bytesWritten = spiWriteCommandAndDataBlocking(flashConfiguration->flashSpiConfiguration, &command, &dataToWrite);

    flashWaitForDone(flashConfiguration->flashSpiConfiguration);
    spiDeinit(flashConfiguration->flashSpiConfiguration);

    return bytesWritten;
}

/* endregion PUBLIC HEADER FUNCTIONS */

/* region INTERNAL HEADER FUNCTIONS */

static void flashEnableWrite(spiConfiguration_t *flashSpiConfiguration) {
    uint8_t cmd[] = {FLASH_ENABLE_WRITE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiWriteCommandBlocking(flashSpiConfiguration, &command);
}

static uint8_t flashReadStatusRegister(spiConfiguration_t *flashSpiConfiguration) {
    uint8_t cmd[] = {FLASH_STATUS_REGISTER};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    uint8_t statusRegister[1];
    data_t statusRegisterData = {.data = statusRegister, .length = sizeof(statusRegister)};

    spiWriteCommandAndReadBlocking(flashSpiConfiguration, &command, &statusRegisterData);
    PRINT_DEBUG("Status Register: 0x%02X", statusRegisterData.data[0]);
    return statusRegisterData.data[0];
}

static flashErrorCode_t flashEraseErrorOccurred(spiConfiguration_t *flashSpiConfiguration) {
    return ((flashReadStatusRegister(flashSpiConfiguration) >> 5) & 1) ? FLASH_ERASE_ERROR : FLASH_NO_ERROR;
}

static void flashWaitForDone(spiConfiguration_t *flashSpiConfiguration) {
    while (flashReadStatusRegister(flashSpiConfiguration) & 0x01) {}
}
/* endregion INTERNAL HEADER FUNCTIONS */
