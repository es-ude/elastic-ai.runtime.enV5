//
// Created by Natalie Maman on 13.04.22.
//
#include "Spi.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include <stdint.h>

void SPI_init_chipselect(const uint8_t cs_pin) {
    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);
}

void SPI_init(spi_inst_t *spi, uint32_t baudrate, const uint8_t cs_pin, const uint8_t sck_pin,
              const uint8_t mosi_pin, const uint8_t miso_pin) {

    SPI_init_chipselect(cs_pin);
    spi_init(spi, baudrate);

    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);
}

void SPI_deinit(spi_inst_t *spi) {
    spi_deinit(spi);
}

void SPI_enable(const uint8_t cs_pin) {
    gpio_put(cs_pin, 0);
}

void SPI_disable(const uint8_t cs_pin) {
    gpio_put(cs_pin, 1);
}

int SPI_read_blocking(spi_inst_t *spi, uint8_t *data_read, uint16_t length_read) {
    int blocks_read = spi_read_blocking(spi, 0, data_read, length_read);
    return blocks_read;
}

int SPI_write_blocking(spi_inst_t *spi, uint8_t *data_to_write, uint16_t length_write) {
    int blocks_written = spi_write_blocking(spi, data_to_write, length_write);
    return blocks_written;
}