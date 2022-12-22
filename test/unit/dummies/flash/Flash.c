#include "flash/Flash.h"
#include <stdint.h>
#include <printf.h>

uint32_t addressSectorErase=0;
uint32_t addressWrite[4];
uint32_t numSectorErase = 0;
uint8_t dataComplete[65536 * 4];
uint32_t numWriteBlocks;

uint8_t* flash;
uint8_t flashLength;

void flashSetData(uint8_t* flashData, uint32_t sizeFlashData){
    flash=flashData;
    flashLength=sizeFlashData;
}

uint8_t flashEraseData(uint32_t address) {
    numSectorErase++;
    addressSectorErase = address;
    return 0;
}

int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length) {
    if (address==0) {
        for (uint16_t i = 0; i < length; i++) {
            data_buffer[i] =flash[i+address];
        }
    }
    return 2;
}
int flashWritePage(uint32_t address, uint8_t *data, uint16_t page_size) {
    addressWrite[numWriteBlocks] = address;
    for (uint16_t i = 0; i < page_size; i++) {
        dataComplete[address + i] = data[i];
    }
    numWriteBlocks++;

    return 0;
}
