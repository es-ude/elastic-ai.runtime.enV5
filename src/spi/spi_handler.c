//
// Created by Natalie Maman on 13.04.22.
//
#include "hardware/spi.h"
#include "spi_handler.h"
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

int flash_read_data(spi_inst_t *spi, const uint8_t address, uint8_t *data_buffer, uint16_t length){
    uint8_t cmd[4]= {
            0x03,
            address >>16,
            address >> 8,
            address
    };
    gpio_put(CS_PIN, 0);
    spi_write_blocking(spi, cmd, 4);
    int read= spi_read_blocking(spi, 0, data_buffer,length);
    gpio_put(CS_PIN, 1);
    return read;
}

void flash_write_enable(spi_inst_t *spi){
    gpio_put(CS_PIN, 0);
    uint8_t cmd = 0x06;
    spi_write_blocking(spi, &cmd, 1);
    gpio_put(CS_PIN,1);

}
int flash_write_page(spi_inst_t *spi,uint32_t address, uint8_t *data, uint16_t page_size ){
    uint8_t cmdbuf[4] = {
            0x02,
            address >> 16,
            address >> 8,
            address
    };
    flash_write_enable(spi);
    gpio_put(CS_PIN, 0);
    spi_write_blocking(spi, cmdbuf, 4);
    int data_wrote= spi_write_blocking(spi, data, page_size);
    gpio_put(CS_PIN, 1);
    return data_wrote;
}