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

/* region PUBLIC HEADER FUNCTIONS */

static uint8_t publicConfig[8];

uint8_t FLASH_BYTES_PER_PAGE;
uint8_t FLASH_BYTES_PER_SECTOR;
uint32_t FLASH_NUMBER_OF_SECTORS;
uint32_t FLASH_NUMBER_OF_BYTES;

void readConfigByLength(flashConfiguration_t *flashConfig, uint8_t registerToRead, uint8_t length) {
    data_t buffer = {.data = publicConfig, .length = length};
    flashReadConfig(flashConfig, registerToRead, &buffer);
}

int getBytesPerSector() {
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    if (!publicConfig[4] == 0x00) {
        printf("FLASH IS NOT SUPPORTED. ABORTING...");
        return 0;
    }
    return 256144;

}
int getBytesPerPage() {
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    if (!publicConfig[4] == 0x00) {
        printf("FLASH IS NOT SUPPORTED. ABORTING...");
        return 0;
    }
    return 512;
}

int getBytesInFlash() {
    // 0x00 means flash has uniform sectors. Only flash modules with uniform sectors are supported.
    if (!publicConfig[4] == 0x00) {
        printf("FLASH IS NOT SUPPORTED. ABORTING...");
        return 0;
    }

    uint32_t oneMB = 1048576;
    // Flash Size: 128MB
    if (publicConfig[1] == 0x20 && publicConfig[2] == 0x18) {
        return 128 * oneMB;
    }

    // Flash Size: 256MB
    if (publicConfig[1] == 0x02 && publicConfig[2] == 0x19) {
        return 256 * oneMB;
    }

}
int getNumberOfSectors() {
    return getBytesInFlash() / getBytesPerSector();
}


void flashInit(flashConfiguration_t *flashConfig) {
    readConfigByLength(flashConfig, FLASH_READ_ID, 8);

    FLASH_BYTES_PER_PAGE = getBytesPerPage(flashConfig);
    FLASH_BYTES_PER_SECTOR = getBytesPerSector(flashConfig);
    FLASH_NUMBER_OF_BYTES = getBytesInFlash(flashConfig);
    FLASH_NUMBER_OF_SECTORS = getNumberOfSectors(flashConfig);
}

int flashReadConfig(flashConfiguration_t *config, commands_t registerToRead, data_t *dataBuffer) {
    uint8_t cmd[] = {registerToRead};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(config->flashSpiConfiguration);
    int readBytes =
        spiWriteCommandAndReadBlocking(config->flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(config->flashSpiConfiguration);
    return readBytes;
}
int flashWriteConfig(flashConfiguration_t *config, uint8_t *configToWrite, size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_CONFIG_REGISTER};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = configToWrite, .length = bytesToWrite};

    spiInit(config->flashSpiConfiguration);
    flashEnableWrite(config->flashSpiConfiguration);

    int bytesWritten =
        spiWriteCommandAndDataBlocking(config->flashSpiConfiguration, &command, &dataToWrite);

    flashWaitForDone(config);
    spiDeinit(config->flashSpiConfiguration);

    return bytesWritten;
}

int flashReadData(flashConfiguration_t *config, uint32_t startAddress, data_t *dataBuffer) {
    uint8_t cmd[] = {FLASH_READ, startAddress >> 16, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(config->flashSpiConfiguration);
    int bytesRead =
        spiWriteCommandAndReadBlocking(config->flashSpiConfiguration, &command, dataBuffer);
    spiDeinit(config->flashSpiConfiguration);
    return bytesRead;
}

flashErrorCode_t flashEraseAll(flashConfiguration_t *config) {
    uint8_t cmd[] = {FLASH_BULK_ERASE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(config->flashSpiConfiguration);
    flashEnableWrite(config->flashSpiConfiguration);

    spiWriteCommandBlocking(config->flashSpiConfiguration, &command);
    sleep_for_ms(33000);
    flashWaitForDone(config);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(config);
    spiDeinit(config->flashSpiConfiguration);

    return eraseError;
}
flashErrorCode_t flashEraseSector(flashConfiguration_t *config, uint32_t address) {
    uint8_t cmd[] = {FLASH_ERASE_SECTOR, address >> 16 & 0xFF, address >> 8 & 0xFF, address & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiInit(config->flashSpiConfiguration);
    flashEnableWrite(config->flashSpiConfiguration);

    spiWriteCommandBlocking(config->flashSpiConfiguration, &command);

    flashWaitForDone(config);
    flashErrorCode_t eraseError = flashEraseErrorOccurred(config);
    spiDeinit(config->flashSpiConfiguration);

    return eraseError;
}

int flashWritePage(flashConfiguration_t *config, uint32_t startAddress, uint8_t *data,
                   size_t bytesToWrite) {
    uint8_t cmd[] = {FLASH_WRITE_PAGE, startAddress >> 16 & 0xFF, startAddress >> 8 & 0xFF,
                     startAddress & 0xFF};
    data_t command = {.data = cmd, .length = sizeof(cmd)};
    data_t dataToWrite = {.data = data, .length = bytesToWrite};

    spiInit(config->flashSpiConfiguration);
    flashEnableWrite(config->flashSpiConfiguration);

    int bytesWritten =
        spiWriteCommandAndDataBlocking(config->flashSpiConfiguration, &command, &dataToWrite);

    flashWaitForDone(config);
    spiDeinit(config->flashSpiConfiguration);

    return bytesWritten;
}

/* endregion PUBLIC HEADER FUNCTIONS */

/* region INTERNAL HEADER FUNCTIONS */

static void flashEnableWrite(spiConfiguration_t *config) {
    uint8_t cmd[] = {FLASH_ENABLE_WRITE};
    data_t command = {.data = cmd, .length = sizeof(cmd)};

    spiWriteCommandBlocking(config, &command);
}

static flashErrorCode_t flashEraseErrorOccurred(flashConfiguration_t *config) {
    uint8_t configRegister;
    data_t buffer = {.data = &configRegister, .length = 1};
    flashReadConfig(config, FLASH_READ_STATUS_REGISTER, &buffer);
    return ((configRegister >> 5) & 1) ? FLASH_ERASE_ERROR : FLASH_NO_ERROR;
}

static void flashWaitForDone(flashConfiguration_t *config) {
    uint8_t configRegister;
    do {
        data_t buffer = {.data = &configRegister, .length = 1};
        flashReadConfig(config, FLASH_READ_STATUS_REGISTER, &buffer);
    } while (configRegister & 0x01);
}
/* endregion INTERNAL HEADER FUNCTIONS */
