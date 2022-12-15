#ifndef ENV5_FLASH_UNITTEST_HEADER
#define ENV5_FLASH_UNITTEST_HEADER

#include <memory.h>
#include <stdint.h>

extern uint32_t addressSectorErase;
extern uint32_t numSectorErase;
extern uint32_t addressWrite[4];
extern uint8_t dataComplete[65536 * 4];
extern uint32_t numWriteBlocks;

void flashSetDataLength(uint32_t numBlocks, uint32_t blockSize);
uint8_t flashEraseData(uint32_t address);
int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length);
int flashWritePage(uint32_t address, uint8_t *data, uint16_t page_size);

#endif /* ENV5_FLASH_UNITTEST_HEADER */
