#ifndef ENV5_FLASH_H
#define ENV5_FLASH_H

#include <stdint.h>
#include "typedefs.h"

void init_flash(uint8_t chip_select, spi_inst_t *spiInst);
int flash_read_id(uint8_t *data_buffer,
                  uint16_t length);


int flash_read_data(uint32_t address,
                    uint8_t *data_buffer,
                    uint16_t length);

uint8_t flash_erase_data( uint32_t address);

int flash_write_page(uint32_t address,
                     uint8_t *data,
                     uint16_t page_size);

#endif //ENV5_FLASH_H
