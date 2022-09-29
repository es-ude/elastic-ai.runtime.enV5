#include "flash.h"
#include "spi/spi_handler.h"
#include "typedefs.h"
#include <stdint.h>

static uint8_t cs_pin;
static spi_inst_t *spi;

void init_flash(uint8_t chip_select, spi_inst_t *spiInst) {
    cs_pin = chip_select;
    spi = spiInst;
}

void flash_write_enable() {
    uint8_t cmd = 0x06;
    SPI_enable(cs_pin);
    SPI_write_blocking(spi, &cmd, 1);
    SPI_disable(cs_pin);
}

int flash_read_id(uint8_t *data_buffer, uint16_t length) {
    uint8_t msg = 0x9F;
    SPI_enable(cs_pin);
    SPI_write_blocking(spi, &msg, 1);
    int num_bytes_read = SPI_read_blocking(spi, data_buffer, length);
    SPI_disable(cs_pin);
    return num_bytes_read;
}

int flash_read_data(const uint32_t address, uint8_t *data_buffer, uint16_t length) {
    uint8_t cmd[4] = {0x03, address >> 16, address >> 8, address};

    SPI_enable(cs_pin);
    SPI_write_blocking(spi, cmd, 4);
    int read = SPI_read_blocking(spi, data_buffer, length);
    SPI_disable(cs_pin);
    return read;
}

uint8_t flash_read_status_reg() {
    uint8_t status_reg;
    uint8_t read_status_register = 0x05;
    SPI_enable(cs_pin);
    SPI_write_blocking(spi, &read_status_register, 1);
    SPI_read_blocking(spi, &status_reg, 1);
    SPI_disable(cs_pin);
    return status_reg;
}

uint8_t erase_error_occured() {
    return ((flash_read_status_reg() >> 5) & 1);
}

void flash_wait_for_done() {
    uint8_t status_reg;
    do {
        status_reg = flash_read_status_reg();
    } while (status_reg & 1);
}

uint8_t flash_erase_data(uint32_t address) {
    //   printf("flash_erase_data()\n");
    uint8_t cmd[4] = {0xD8, address >> 16, address >> 8, address};

    //   printf("cmd 0x%02X,0x%02X,0x%02X,0x%02X\n", cmd[0],cmd[1],cmd[2],cmd[3]);
    flash_write_enable();
    SPI_enable(cs_pin);
    SPI_write_blocking(spi, cmd, 4);
    SPI_disable(cs_pin);
    //    printf("flash & spi cmds done\n");
    flash_wait_for_done();
    //   printf("flash_wait_for_done() finished\n");
    uint8_t status = erase_error_occured();
    //   printf("erase error status code: %u\n", status);
    return status;
}

int flash_write_page(uint32_t address, uint8_t *data, uint16_t page_size) {
    uint8_t cmd[4] = {0x02, address >> 16, address >> 8, address};
    flash_write_enable();
    SPI_enable(cs_pin);
    SPI_write_blocking(spi, cmd, 4);
    int data_wrote = SPI_write_blocking(spi, data, page_size);
    SPI_disable(cs_pin);
    flash_wait_for_done();
    return data_wrote;
}
