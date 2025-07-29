/* Datasheet:
 *    https://www.mouser.de/datasheet/2/196/Infineon_S25FL128SS25FL256S_128_Mb__16_MB_256_Mb__-3166946.pdf
 *
 *  IMPORTANT:
 *    Because the QXSPI-library uses the same SPI module,
 *    before every operation the module has to be initialized!
 */

#define SOURCE_FILE "FLASH"

#include <stdint.h>
#include <stdio.h>

#include "FlashInternal.h"
#include "eai/Common.h"
#include "eai/flash/Flash.h"
#include "eai/flash/FlashTypedefs.h"
#include "eai/hal/Sleep.h"
#include "eai/hal/Spi.h"
#include "eai/hal/SpiTypedefs.h"

/* region PUBLIC HEADER FUNCTIONS */
void flashInit(flashConfiguration_t *flashConfig) {
    uint8_t config[33];
    data_t configBuffer = {.data = config, .length = 33};
    flashReadConfig(flashConfig, FLASH_READ_ID, &configBuffer);

    flashConfig->size = calculateBytesInFlash(config);
    flashConfig->bytesPerPage = calculateBytesPerPage(config);
    flashConfig->bytesPerSector = calculateBytesPerSector(config);
    flashConfig->numberOfSectors = (int)flashConfig->size / (int)flashConfig->bytesPerSector;
}

size_t flashGetBytesPerPage(flashConfiguration_t *flashConfig) {
    return flashConfig->bytesPerPage;
}

size_t flashGetBytesPerSector(flashConfiguration_t *flashConfig) {
    return flashConfig->bytesPerSector;
}

size_t flashGetNumberOfBytes(flashConfiguration_t *flashConfig) {
    return flashConfig->size;
}

size_t flashGetNumberOfSectors(flashConfiguration_t *flashConfig) {
    return flashConfig->size / flashConfig->bytesPerSector;
}

int flashReadConfig(flashConfiguration_t *flashConfig, const commands_t registerToRead,
                    data_t *dataBuffer) {
    uint8_t cmd[] = {registerToRead};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->spiConfiguration);
    int readBytes =
        spiWriteCommandAndReadBlocking(flashConfig->spiConfiguration, &command, dataBuffer);
    spiDeinit(flashConfig->spiConfiguration);
    return readBytes;
}

int flashWriteConfig(flashConfiguration_t *flashConfig, uint8_t *configToWrite,
                     const size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_CONFIG_REGISTER};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = configToWrite, .length = bytesToWrite};

    spiInit(flashConfig->spiConfiguration);
    flashEnableWrite(flashConfig->spiConfiguration);

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashConfig->spiConfiguration, &command, &dataToWrite);

    flashWaitForDone(flashConfig);
    spiDeinit(flashConfig->spiConfiguration);

    return bytesWritten;
}

int flashReadData(flashConfiguration_t *flashConfig, const uint32_t startAddress,
                  data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->spiConfiguration);
    int bytesRead =
        spiWriteCommandAndReadBlocking(flashConfig->spiConfiguration, &command, dataBuffer);
    spiDeinit(flashConfig->spiConfiguration);
    return bytesRead;
}

flashErrorCode_t flashEraseAll(flashConfiguration_t *flashConfig) {
    uint8_t cmd[] = {FLASH_BULK_ERASE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->spiConfiguration);
    flashEnableWrite(flashConfig->spiConfiguration);

    spiWriteCommandBlocking(flashConfig->spiConfiguration, &command);
    sleep_for_ms(33000);
    flashWaitForDone(flashConfig);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(flashConfig);
    spiDeinit(flashConfig->spiConfiguration);

    return eraseError;
}

flashErrorCode_t flashEraseSector(flashConfiguration_t *flashConfig, const uint32_t address) {
    uint8_t cmd[] = {FLASH_ERASE_SECTOR, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->spiConfiguration);
    flashEnableWrite(flashConfig->spiConfiguration);

    spiWriteCommandBlocking(flashConfig->spiConfiguration, &command);

    flashWaitForDone(flashConfig);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(flashConfig);
    spiDeinit(flashConfig->spiConfiguration);

    return eraseError;
}

int flashWritePage(flashConfiguration_t *flashConfig, const uint32_t startAddress, uint8_t *data,
                   const size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(flashConfig->spiConfiguration);
    flashEnableWrite(flashConfig->spiConfiguration);

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashConfig->spiConfiguration, &command, &dataToWrite);

    flashWaitForDone(flashConfig);
    spiDeinit(flashConfig->spiConfiguration);

    return bytesWritten;
}
/* endregion PUBLIC HEADER FUNCTIONS */

/* region INTERNAL HEADER FUNCTIONS */

static void flashEnableWrite(spiConfiguration_t *spiConfig) {
    uint8_t cmd[] = {FLASH_ENABLE_WRITE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiWriteCommandBlocking(spiConfig, &command);
}

static flashErrorCode_t flashEraseErrorOccurred(flashConfiguration_t *flashConfig) {
    uint8_t configRegister;
    data_t buffer = {.data = &configRegister, .length = 1};
    flashReadConfig(flashConfig, FLASH_READ_STATUS_REGISTER, &buffer);
    return ((configRegister >> 5) & 1) ? FLASH_ERASE_ERROR : FLASH_NO_ERROR;
}

static void flashWaitForDone(flashConfiguration_t *flashConfig) {
    uint8_t configRegister;
    do {
        data_t buffer = {.data = &configRegister, .length = 1};
        flashReadConfig(flashConfig, FLASH_READ_STATUS_REGISTER, &buffer);
    } while (configRegister & 0x01);
}

static size_t calculateBytesPerSector(const uint8_t *config) {
    if (config[4] == 0x00) {
        return 262144;
    }

    PRINT("FLASH IS NOT SUPPORTED. ABORTING BYTES PER SECTOR...\n");
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    return 0;
}

static size_t calculateBytesPerPage(const uint8_t *config) {
    if (config[32] == 0x09) {
        return 512;
    }

    printf("FLASH IS NOT SUPPORTED. ABORTING BYTES PER PAGE...\n");
    // 256B pages only in hybrid flash used
    return 0;
}

static size_t calculateBytesInFlash(const uint8_t *config) {
    if (config[1] == 0x20 && config[2] == 0x18) {
        return 134217728; //! Flash Size: 128MB
    }

    if (config[1] == 0x02 && config[2] == 0x19) {
        return 268435456; //! Flash Size: 256MB
    }

    printf("FLASH IS NOT SUPPORTED. ABORTING NO CASE BYTES IN FLASH...\n");
    return 0;
}

/* endregion INTERNAL HEADER FUNCTIONS */
