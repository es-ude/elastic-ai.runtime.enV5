//
// Created by Natalie Maman on 01.06.22.
//

#ifndef ENV5_FLASH_H
#define ENV5_FLASH_H

#include <memory.h>
#include <stdint.h>
uint32_t addressBlockErase;
uint32_t addressWrite[4];
uint8_t dataComplete[256*4];

void setDataLength(uint32_t numBlocks, uint32_t blockSize);
uint8_t flash_erase_data(uint32_t address);

int flash_write_page(uint32_t address,
                     uint8_t *data,
                     uint16_t page_size);
#endif //ENV5_FLASH_TEST_H
