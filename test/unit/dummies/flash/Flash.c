#include "flash/Flash.h"
#include <stdint.h>

uint32_t addressBlockErase;
uint32_t addressWrite[4];
uint8_t dataComplete[256 * 4];

uint32_t numBlockErase = 0;
uint32_t numWriteBlocks = 0;

uint8_t flash_erase_data(uint32_t address) {
    numBlockErase++;
    addressBlockErase = address;
    return 0;
}

int flash_read_data(uint32_t address, uint8_t *data_buffer, uint16_t length) {
    return 2;
}
int flash_write_page(uint32_t address, uint8_t *data, uint16_t page_size) {
    addressWrite[numWriteBlocks] = address;
    for (uint16_t i = 0; i < page_size; i++) {
        dataComplete[address + i] = data[i];
    }
    numWriteBlocks++;

    return 0;
}
