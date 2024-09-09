/* Datasheet:
 *    https://www.mouser.de/datasheet/2/196/Infineon_S25FL128SS25FL256S_128_Mb__16_MB_256_Mb__-3166946.pdf
 *
 *  IMPORTANT:
 *    Because the QXSPI-library uses the same SPI module,
 *    before every operation the module has to be initialized!
 */

#define SOURCE_FILE "FLASH"

#include <stdint.h>

#include "Flash.h"
#include "FlashInternal.h"
#include "FlashTypedefs.h"
#include "Sleep.h"
#include "Spi.h"
#include "SpiTypedefs.h"
#include "stdio.h"

static uint8_t configReadback[33];
static uint32_t FLASH_BYTES_PER_PAGE;
static uint32_t FLASH_BYTES_PER_SECTOR;
static uint32_t FLASH_NUMBER_OF_SECTORS;
static uint32_t FLASH_NUMBER_OF_BYTES;

/* region PUBLIC HEADER FUNCTIONS */
void flashInit(flashConfiguration_t *flashConfig) {
    const uint8_t *config = readConfigByLength(flashConfig, FLASH_READ_ID, 33);

    calculateBytesPerPage(config);
    calculateBytesPerSector(config);
    calculateBytesInFlash(config);
    calculateNumberOfSectors(config);
}

uint32_t flashGetBytesPerPage() {
    return FLASH_BYTES_PER_PAGE;
}
uint32_t flashGetBytesPerSector() {
    return FLASH_BYTES_PER_SECTOR;
}
uint32_t flashGetNumberOfBytes() {
    return FLASH_NUMBER_OF_BYTES;
}
uint32_t flashGetNumberOfSectors() {
    return FLASH_NUMBER_OF_SECTORS;
}

int flashReadConfig(flashConfiguration_t *flashConfig, const commands_t registerToRead,
                    data_t *dataBuffer) {
    uint8_t cmd[] = {registerToRead};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->flashSpiConfiguration);
    int readBytes =
        spiWriteCommandAndReadBlocking(flashConfig->flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(flashConfig->flashSpiConfiguration);
    return readBytes;
}

uint8_t *readConfigByLength(flashConfiguration_t *flashConfig, const uint8_t registerToRead,
                            const uint8_t length) {
    data_t buffer = {.data = configReadback, .length = length};
    flashReadConfig(flashConfig, registerToRead, &buffer);
    return configReadback;
}

int flashWriteConfig(flashConfiguration_t *flashConfig, uint8_t *configToWrite,
                     const size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_CONFIG_REGISTER};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = configToWrite, .length = bytesToWrite};

    spiInit(flashConfig->flashSpiConfiguration);
    flashEnableWrite(flashConfig->flashSpiConfiguration);

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashConfig->flashSpiConfiguration, &command, &dataToWrite);

    flashWaitForDone(flashConfig);
    spiDeinit(flashConfig->flashSpiConfiguration);

    return bytesWritten;
}

int flashReadData(flashConfiguration_t *flashConfig, const uint32_t startAddress,
                  data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->flashSpiConfiguration);
    int bytesRead =
        spiWriteCommandAndReadBlocking(flashConfig->flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(flashConfig->flashSpiConfiguration);
    return bytesRead;
}

flashErrorCode_t flashEraseAll(flashConfiguration_t *flashConfig) {
    uint8_t cmd[] = {FLASH_BULK_ERASE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->flashSpiConfiguration);
    flashEnableWrite(flashConfig->flashSpiConfiguration);

    spiWriteCommandBlocking(flashConfig->flashSpiConfiguration, &command);
    sleep_for_ms(33000);
    flashWaitForDone(flashConfig);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(flashConfig);
    spiDeinit(flashConfig->flashSpiConfiguration);

    return eraseError;
}

flashErrorCode_t flashEraseSector(flashConfiguration_t *flashConfig, const uint32_t address) {
    uint8_t cmd[] = {FLASH_ERASE_SECTOR, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(flashConfig->flashSpiConfiguration);
    flashEnableWrite(flashConfig->flashSpiConfiguration);

    spiWriteCommandBlocking(flashConfig->flashSpiConfiguration, &command);

    flashWaitForDone(flashConfig);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(flashConfig);
    spiDeinit(flashConfig->flashSpiConfiguration);

    return eraseError;
}

int flashWritePage(flashConfiguration_t *flashConfig, const uint32_t startAddress, uint8_t *data,
                   const size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(flashConfig->flashSpiConfiguration);
    flashEnableWrite(flashConfig->flashSpiConfiguration);

    int bytesWritten =
        spiWriteCommandAndDataBlocking(flashConfig->flashSpiConfiguration, &command, &dataToWrite);

    flashWaitForDone(flashConfig);
    spiDeinit(flashConfig->flashSpiConfiguration);

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

static void calculateBytesPerSector(const uint8_t *config) {
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    if (config[4] != 0x00) {
        printf("FLASH IS NOT SUPPORTED. ABORTING BYTES PER SECTOR...\n");
        FLASH_BYTES_PER_SECTOR = 0;
        return;
    }
    FLASH_BYTES_PER_SECTOR = 262144;
}
static void calculateBytesPerPage(const uint8_t *config) {
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    if (config[32] == 0x08) {
        FLASH_BYTES_PER_PAGE = 256;
        return;
    }
    if (config[32] == 0x09) {
        FLASH_BYTES_PER_PAGE = 512;
        return;
    }
    printf("FLASH IS NOT SUPPORTED. ABORTING BYTES PER PAGE...\n");
    FLASH_BYTES_PER_PAGE = 0;
}
static void calculateBytesInFlash(const uint8_t *config) {
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    if (config[4] != 0x00) {
        printf("FLASH IS NOT SUPPORTED. ABORTING BYTES IN FLASH...\n");
        FLASH_NUMBER_OF_BYTES = 0;
        return;
    }

    const int oneMB = 1048576;
    // Flash Size: 128MB
    if (config[1] == 0x20 && config[2] == 0x18) {
        FLASH_NUMBER_OF_BYTES = 128 * oneMB;
        return;
    }

    // Flash Size: 256MB
    if (config[1] == 0x02 && config[2] == 0x19) {
        FLASH_NUMBER_OF_BYTES = 256 * oneMB;
        return;
    }

    printf("FLASH IS NOT SUPPORTED. ABORTING NO CASE BYTES IN FLASH...\n");
    FLASH_NUMBER_OF_BYTES = 0;
}
static void calculateNumberOfSectors() {
    FLASH_NUMBER_OF_SECTORS = FLASH_NUMBER_OF_BYTES / FLASH_BYTES_PER_SECTOR;
}
/* endregion INTERNAL HEADER FUNCTIONS */
