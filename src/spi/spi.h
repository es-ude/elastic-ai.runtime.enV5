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

int spi_read_data(  spi_inst_t *spi,
                    const uint8_t address,
                    uint8_t *data_buffer,
                    uint8_t length);


void spi_get_ID();

void spi_write_data();



#endif //ENV5_UNITTESTS_SPI_H
