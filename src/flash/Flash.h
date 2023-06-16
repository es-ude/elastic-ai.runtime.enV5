#ifndef ENV5_FLASH_HEADER
#define ENV5_FLASH_HEADER

#include "FlashTypedefs.h"
#include "spi/SpiTypedefs.h"
#include <stdint.h>
#include "FileIterator.h"

void flashInit(uint8_t chip_select, spi_inst_t *spiInst);

int flashReadId(uint8_t *data_buffer, uint16_t length);

int flashReadData(uint32_t address, uint8_t *data_buffer, uint16_t length);

uint8_t flashEraseData(uint32_t address);

flashWriteError_t flashWrite(uint32_t address, dataBlock_t* dataBlock);

#endif /* ENV5_FLASH_HEADER */
