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

/*TODO: in main.c
uint32_t FLASH_BYTES_PER_PAGE = FLASH_BYTES_PER_SECTOR
congig.c
#define FLASH_BYTES_PER_PAGE 256
 sensorconfig, busconfig (doku)=networkconfig, flashconfig, fpgaconfig
 
 fpgaconfigurationhandler nochmal anschauen
*/

/* region PUBLIC HEADER FUNCTIONS */


int flashReadId(spiConfiguration_t *flashSpiConfiguration, data_t *dataBuffer) {
    uint8_t cmd[1] = {FLASH_READ_ID};
    data_t command = {.data = cmd, .length = 1};

    spiInit(flashSpiConfiguration);
    int readBytes = spiWriteCommandAndReadBlocking(flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(flashSpiConfiguration);
    return readBytes;
}
int flashReadData(spiConfiguration_t *flashSpiConfiguration, uint32_t startAddress, data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpiConfiguration);
    int bytesRead = spiWriteCommandAndReadBlocking(flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(flashSpiConfiguration);
    return bytesRead;
}

flashErrorCode_t flashEraseAll(spiConfiguration_t *flashSpiConfiguration) {
    uint8_t cmd[] = {FLASH_BULK_ERASE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpiConfiguration);
    flashEnableWrite(flashSpiConfiguration);

    spiWriteCommandBlocking(flashSpiConfiguration, &command);
    sleep_for_ms(33000);
    flashWaitForDone();
    flashErrorCode_t eraseError = flashEraseErrorOccurred(NULL, 0);
    spiDeinit(flashSpiConfiguration);

    return eraseError;
}
flashErrorCode_t flashEraseSector(spiConfiguration_t *flashSpiConfiguration, uint32_t address) {
    uint8_t cmd[] = {FLASH_ERASE_SECTOR, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashSpiConfiguration);
    flashEnableWrite(flashSpiConfiguration);

    spiWriteCommandBlocking(flashSpiConfiguration, &command);

    flashWaitForDone();
    flashErrorCode_t eraseError = flashEraseErrorOccurred(NULL, 0);
    spiDeinit(flashSpiConfiguration);

    return eraseError;
}

int flashWritePage(spiConfiguration_t *flashSpiConfiguration, uint32_t startAddress, uint8_t *data,
                   size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(flashSpiConfiguration);
    flashEnableWrite(flashSpiConfiguration);

    int bytesWritten = spiWriteCommandAndDataBlocking(flashSpiConfiguration, &command, &dataToWrite);

    flashWaitForDone();
    spiDeinit(flashSpiConfiguration);

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

static flashErrorCode_t flashEraseErrorOccurred(spiConfiguration_t *flashSpiConfiguration, ) {
    return ((flashReadStatusRegister(NULL) >> 5) & 1) ? FLASH_ERASE_ERROR : FLASH_NO_ERROR;
}

static void flashWaitForDone(void) {
    while (flashReadStatusRegister(NULL) & 0x01) {}
}
/* endregion INTERNAL HEADER FUNCTIONS */
