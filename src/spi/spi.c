//
// Created by Natalie Maman on 13.04.22.
//
#include "hardware/spi.h"
#include "spi.h"
#include <stdint.h>
#include "hardware/gpio.h"


void spi_init_handler(spi_inst_t *spi, uint32_t baudrate) {
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 1);
    spi_init(spi, baudrate);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(MISO_PIN, GPIO_FUNC_SPI);
}

int spi_read_data(spi_inst_t *spi,
                  const uint8_t address,
                  uint8_t *data_buffer,
                  uint8_t length) {
    uint8_t msg = 0x9F;
    gpio_put(CS_PIN, 0);
    spi_write_blocking(spi, &msg, 1);
    int num_bytes_read = spi_read_blocking(spi, 0, data_buffer, length);
    gpio_put(CS_PIN, 1);

    return num_bytes_read;
}
