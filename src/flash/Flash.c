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

static spiConfiguration_t *flashSpi;
static uint8_t flashChipSelectPin;

/* region PUBLIC HEADER FUNCTIONS */

void flashInit(spiConfiguration_t *spiConfiguration) {
    flashChipSelectPin = spiConfiguration->csPin;
    flashSpi = spiConfiguration;
}

int flashReadConfig(commands_t registerToRead, data_t *dataBuffer) {
    uint8_t cmd[] = {registerToRead};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpi);
    int readBytes =
        spiWriteCommandAndReadBlocking(flashSpi, &command, dataBuffer);
    spiDeinit(flashSpi);
    return readBytes;
}
int flashWriteConfig(uint8_t *configToWrite, size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_CONFIG_REGISTER};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = configToWrite, .length = bytesToWrite};

    spiInit(flashSpi, flashChipSelectPin);
    flashEnableWrite();

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashSpi, flashChipSelectPin, &command, &dataToWrite);

    flashWaitForDone();
    spiDeinit(flashSpi, flashChipSelectPin);

    return bytesWritten;
}

int flashReadData(uint32_t startAddress, data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpi);
    int bytesRead =
        spiWriteCommandAndReadBlocking(flashSpi, &command, dataBuffer);
    spiDeinit(flashSpi);
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

int flashWritePage(uint32_t startAddress, uint8_t *data, size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(flashSpi, flashChipSelectPin);
    flashEnableWrite();

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashSpi, &command, &dataToWrite);

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

static flashErrorCode_t flashEraseErrorOccurred(void) {
    uint8_t configRegister;
    data_t buffer = {.data = &configRegister, .length = 1};
    flashReadConfig(FLASH_READ_STATUS_REGISTER, &buffer);
    return ((configRegister >> 5) & 1) ? FLASH_ERASE_ERROR : FLASH_NO_ERROR;
}

static void flashWaitForDone(void) {
    uint8_t configRegister;
    do {
        data_t buffer = {.data = &configRegister, .length = 1};
        flashReadConfig(FLASH_READ_STATUS_REGISTER, &buffer);
    } while (configRegister & 0x01);
}
/* endregion INTERNAL HEADER FUNCTIONS */
