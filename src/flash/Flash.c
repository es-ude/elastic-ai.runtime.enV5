 #define SOURCE_FILE "FLASH"

#include "Flash.h"
#include "FlashInternal.h"
#include "spi/Spi.h"
#include "pico/printf.h"
#include <stdint.h>

static uint8_t flashCsPin;
static spi_inst_t *flashSpi;

void flashInit(uint8_t chip_select, spi_inst_t *spiInst) {
    flashCsPin = chip_select;
    flashSpi = spiInst;
}

void flashWriteEnable(void) {
    uint8_t cmd = 0x06;
    spiWriteSingleCmd(flashSpi, flashCsPin, &cmd, 1);
}

int flashReadId(uint8_t *data_buffer, uint16_t length) {
    uint8_t cmd = 0x9F;
    return spiReadBlocking(flashSpi, flashCsPin, &cmd, 1, data_buffer, length);
}

int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length) {
    uint8_t cmd[4] = {0x03, address >> 16, address >> 8, address};
    return spiReadBlocking(flashSpi, flashCsPin, cmd, 4, data_buffer, length);
}

uint8_t flashReadStatusReg(void) {
    uint8_t statusReg;
    uint8_t readStatusRegister = 0x05;
    spiReadBlocking(flashSpi, flashCsPin, &readStatusRegister, 1, &statusReg, 1);
    return statusReg;
}

uint8_t flashEraseErrorOccurred(void) {
    flashEraseErrorCode_t flashEraseErrorCode = ((flashReadStatusReg() >> 5) & 1);
    return flashEraseErrorCode;
}

void flashWaitForDone(void) {
    uint8_t statusReg;
    do {
        statusReg = flashReadStatusReg();
    } while (statusReg & 1);
}
uint8_t flashEraseData(uint32_t address) {
    uint8_t cmd[4] = {0xD8, address >> 16, address >> 8, address};
    flashWriteEnable();
    spiWriteSingleCmd(flashSpi, flashCsPin, cmd, 4);
    
    flashWaitForDone();
    flashEraseErrorCode_t status = flashEraseErrorOccurred();
    return status;
}

int flashWritePage(uint32_t address, uint8_t *data, uint16_t page_size) {
    uint8_t cmd[4] = {0x02, address >> 16, address >> 8, address};
    flashWriteEnable();
    int dataWrote = spiWriteBlocking(flashSpi, flashCsPin, cmd, 4, data, page_size);
    flashWaitForDone();
    return dataWrote;
}