//
// Created by Natalie Maman on 13.04.22.
//
#ifndef ENV5_UNITTESTS_SPI_H
#define ENV5_UNITTESTS_SPI_H

#include "hardware/spi.h"


#define CS_PIN 1
#define MISO_PIN 0
#define MOSI_PIN 3
#define SCK_PIN 2

void spi_init_handler(spi_inst_t *spi, uint32_t baudrate);

uint8_t flash_erase_data(spi_inst_t *spi,
               const uint32_t address);
int spi_read_id(  spi_inst_t *spi,
                    uint8_t *data_buffer,
                    uint16_t length);

int flash_read_data(spi_inst_t *spi, const uint32_t address, uint8_t *data_buffer, uint16_t length);

int flash_write_page(spi_inst_t *spi,uint32_t address, uint8_t *data, uint16_t page_size );



#endif //ENV5_UNITTESTS_SPI_H
