#ifndef ENV5_FLASH_UNITTEST_HEADER
#define ENV5_FLASH_UNITTEST_HEADER

#include "FlashTypedefs.h"
#include <memory.h>
#include <stdint.h>

uint32_t addressSectorErase;
uint32_t numSectorErase;
uint32_t addressWrite[5];
uint8_t dataComplete[FLASH_SECTOR_SIZE * 4];
uint32_t numWriteBlocks;



void flashSetData(uint8_t *flashData, uint32_t sizeFlashData);
uint8_t flashEraseData(uint32_t address);
int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length);
int flashWritePage(uint32_t address, uint8_t *data, uint16_t page_size);

#endif /* ENV5_FLASH_UNITTEST_HEADER */
