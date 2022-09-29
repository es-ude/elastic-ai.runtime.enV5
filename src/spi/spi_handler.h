//
// Created by Natalie Maman on 13.04.22.
//
#ifndef ENV5_UNITTESTS_SPI_H
#define ENV5_UNITTESTS_SPI_H

#include "hardware/spi.h"

void SPI_init(spi_inst_t *spi, uint32_t baudrate, uint8_t cs_pin, uint8_t sck_pin, uint8_t mosi_pin,
              uint8_t miso_pin);

void SPI_deinit(spi_inst_t *spi);

void SPI_enable(uint8_t cs_pin);

void SPI_disable(uint8_t cs_pin);

int SPI_read_blocking(spi_inst_t *spi, uint8_t *data_read, uint16_t length_read);

int SPI_write_blocking(spi_inst_t *spi, uint8_t *data_to_write, uint16_t length_write);

#endif // ENV5_UNITTESTS_SPI_H
