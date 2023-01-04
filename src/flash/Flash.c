#define SOURCE_FILE "FLASH"

#include "Flash.h"
#define INCLUDE_ENV5_FLASH_INTERNAL_H
#include "FlashInternal.h"
#include "spi/Spi.h"
#include <stdint.h>

static uint8_t flashCsPin;
static spi_inst_t *flashSpi;

void flashInit(uint8_t chip_select, spi_inst_t *spiInst) {
    flashCsPin = chip_select;
    flashSpi = spiInst;
}

void flashWriteEnable(void) {
    uint8_t cmd = 0x06;
    spiEnable(flashCsPin);
    spiWriteBlocking(flashSpi, &cmd, 1);
    spiDisable(flashCsPin);
}

int flashReadId(uint8_t *data_buffer, uint16_t length) {
    uint8_t msg = 0x9F;
    spiEnable(flashCsPin);
    spiWriteBlocking(flashSpi, &msg, 1);
    int numBytesRead = spiReadBlocking(flashSpi, data_buffer, length);
    spiDisable(flashCsPin);
    return numBytesRead;
}

int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length) {
    uint8_t cmd[4] = {0x03, address >> 16, address >> 8, address};

    spiEnable(flashCsPin);
    spiWriteBlocking(flashSpi, cmd, 4);
    int read = spiReadBlocking(flashSpi, data_buffer, length);
    spiDisable(flashCsPin);
    return read;
}

uint8_t flashReadStatusReg(void) {
    uint8_t statusReg;
    uint8_t readStatusRegister = 0x05;
    spiEnable(flashCsPin);
    spiWriteBlocking(flashSpi, &readStatusRegister, 1);
    spiReadBlocking(flashSpi, &statusReg, 1);
    spiDisable(flashCsPin);
    return statusReg;
}

uint8_t flashEraseErrorOccurred(void) {
    return ((flashReadStatusReg() >> 5) & 1);
}

void flashWaitForDone(void) {
    uint8_t statusReg;
    do {
        statusReg = flashReadStatusReg();
    } while (statusReg & 1);
}

uint8_t flashEraseData(uint32_t address) {
    //   printf("flashEraseData()\n");
    uint8_t cmd[4] = {0xD8, address >> 16, address >> 8, address};

    //   printf("cmd 0x%02X,0x%02X,0x%02X,0x%02X\n", cmd[0],cmd[1],cmd[2],cmd[3]);
    flashWriteEnable();
    spiEnable(flashCsPin);
    spiWriteBlocking(flashSpi, cmd, 4);
    spiDisable(flashCsPin);
    //    printf("flash & spi cmds done\n");
    flashWaitForDone();
    //   printf("flashWaitForDone() finished\n");
    uint8_t status = flashEraseErrorOccurred();
    //   printf("erase error status code: %u\n", status);
    return status;
}

int flashWritePage(uint32_t address, uint8_t *data, uint16_t page_size) {
    uint8_t cmd[4] = {0x02, address >> 16, address >> 8, address};
    flashWriteEnable();
    spiEnable(flashCsPin);
    spiWriteBlocking(flashSpi, cmd, 4);
    int dataWrote = spiWriteBlocking(flashSpi, data, page_size);
    spiDisable(flashCsPin);
    flashWaitForDone();
    return dataWrote;
}
