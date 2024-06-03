#define SOURCE_FILE "FLASH-HWTEST"

#include <stdio.h>

#include "hardware/spi.h"
#include "pico/bootrom.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "Common.h"
#include "Flash.h"
#include "Spi.h"
#include "enV5HwController.h"

spi_t spiConfig = {.sckPin = 2, .misoPin = 0, .mosiPin = 3, .baudrate = 1000 * 1000, .spi = spi0};
static const uint32_t startAddress = 0x00000000;
const uint32_t pageLimit = 5;
static const uint8_t csPin = 1;

void initializeConsoleOutput(void) {
    stdio_init_all();
    while ((!stdio_usb_connected())) {
        // wait for serial console to connect
    }
}

void initializeHardware(void) {
    env5HwInit();
    env5HwFpgaPowersOff();

    spiInit(&spiConfig, csPin);
    flashInit(&spiConfig, csPin);
}
void enableQuadSPI(void) {
    uint8_t config[] = {0x00, 0x02};
    flashWriteConfig(config, sizeof(config));
}
void readConfig(uint8_t registerToRead) {
    volatile uint8_t configRegister;
    data_t buffer = {.data = &configRegister, .length = 1};
    flashReadConfig(registerToRead, &buffer);
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
