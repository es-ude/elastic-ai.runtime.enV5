#define SOURCE_FILE "FLASH-HWTEST"

#include <stdio.h>

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "Spi.h"

spiConfiguration_t spiToFlashConfig = {.sckPin = FLASH_SPI_CLOCK,
                                       .misoPin = FLASH_SPI_MISO,
                                       .mosiPin = FLASH_SPI_MOSI,
                                       .baudrate = FLASH_SPI_BAUDRATE,
                                       .spiInstance = FLASH_SPI_MODULE,
                                       .csPin = FLASH_SPI_CS};
flashConfiguration_t flashConfig;

void initializeFlashConfig() {
    flashInit(&spiToFlashConfig);
    flashConfig.flashSpiConfiguration = &spiToFlashConfig;
    flashConfig.flashBytesPerPage = flashGetBytesPerPage();
    flashConfig.flashBytesPerSector = flashGetBytesPerSector();
}

static const uint32_t startAddress = 0x00000000;
const uint32_t pageLimit = 5;

void initializeConsoleOutput(void) {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        // wait for serial console to connect
    }
}

void initializeHardware(void) {
    env5HwControllerInit();
    env5HwControllerFpgaPowersOff();
    spiInit(&spiToFlashConfig);
    initializeFlashConfig();
}
void enableQuadSPI(void) {
    uint8_t config[] = {0x00, 0x02};
    flashWriteConfig(&spiToFlashConfig, config, sizeof(config));
}
void readConfig(uint8_t registerToRead) {
    uint8_t configRegister;
    data_t buffer = {.data = &configRegister, .length = 1};
    flashReadConfig(&spiToFlashConfig, registerToRead, &buffer);
    PRINT("CONFIG: 0x%02X", configRegister);
}

_Noreturn void runTest(void) {
    while (1) {
        char input = getchar_timeout_us(UINT32_MAX);

        switch (input) {
        case 's':
            readConfig(FLASH_READ_STATUS_REGISTER);
            break;
        case 'c':
            readConfig(FLASH_READ_CONFIG_REGISTER);
            break;
        case 'q':
            enableQuadSPI();
            break;
        case 'z':
            printf("Bytes per sector: %i\n", flashGetBytesPerSector());
            printf("Bytes per page: %i\n", flashGetBytesPerPage());
            printf("Total number of bytes: %i\n", flashGetNumberOfBytes());
            printf("Number of sectors: %i\n", flashGetNumberOfSectors());
            break;
        case 'x':
            printf("Bytes per sector: %i\n", flashConfig.flashBytesPerSector);
            printf("Bytes per page: %i\n", flashConfig.flashBytesPerPage);
            break;
        default:
            PRINT("Waiting ...");
        }
    }
}

int main() {
    initializeConsoleOutput();
    initializeHardware();
    runTest();
}
