//
// Created by Natalie Maman on 11.05.22.
//

#ifndef ENV5_FLASH_H
#define ENV5_FLASH_H
#include <stdint.h>
#include "typedefs.h"



//void flash_set_cs_pin(const uint8_t* chip_select);
int flash_read_id(spi_inst_t *spi,const uint8_t cs_pin,
                  uint8_t *data_buffer,
                  uint16_t length);

int flash_read_data(spi_inst_t *spi,const uint8_t cs_pin, const uint32_t address, uint8_t *data_buffer, uint16_t length);
uint8_t flash_erase_data(spi_inst_t *spi,const uint8_t cs_pin, uint32_t address);
int flash_write_page(spi_inst_t *spi,const uint8_t cs_pin, uint32_t address, uint8_t *data, uint16_t page_size );
#endif //ENV5_FLASH_H
