//
// Created by Natalie Maman on 05.05.22.
//

#include <stdint.h>
#include "spi/spi_handler.h"
#include "flash.h"
//#include "typedefs.h"
#include "hardware/spi.h"
//uint8_t cs_pin;

void flash_set_cs_pin(const uint8_t* chip_select){
   // cs_pin=chip_select;
}

void flash_write_enable(spi_inst_t *spi, const uint8_t cs_pin){
    uint8_t cmd = 0x06;
    SPI_write_blocking(spi,cs_pin, &cmd, 1);

}
int flash_read_id(spi_inst_t *spi,const uint8_t cs_pin,
                uint8_t *data_buffer,
                uint16_t length) {
    uint8_t msg = 0x9F;
    SPI_write_blocking(spi,cs_pin, &msg, 1);
    int num_bytes_read = SPI_read_blocking(spi, cs_pin, data_buffer, length);
    return num_bytes_read;
}

int flash_read_data(spi_inst_t *spi, const uint8_t cs_pin, const uint32_t address, uint8_t *data_buffer, uint16_t length){
    uint8_t cmd[4]= {
            0x03,
            address >>16,
            address >> 8,
            address
    };
    SPI_write_blocking(spi,cs_pin, cmd, 4);
    int read= SPI_read_blocking(spi, cs_pin, data_buffer,length);
    return read;
}




uint8_t flash_read_status_reg(spi_inst_t *spi, const uint8_t cs_pin){
    uint8_t status_reg;
    uint8_t read_status_register=0x05;
    SPI_write_blocking(spi,cs_pin, &read_status_register, 1);
    SPI_read_blocking(spi, cs_pin, &status_reg, 1);
    return status_reg;
}
uint8_t flash_erase_succeeded(spi_inst_t *spi, const uint8_t cs_pin) {
    return ((flash_read_status_reg(spi, cs_pin) >> 3) & 1);
}

void flash_wait_for_done(spi_inst_t *spi, const uint8_t cs_pin){
    uint8_t status_reg;
    do{
        status_reg=flash_read_status_reg(spi, cs_pin);
    }while(status_reg & 1);

}
uint8_t flash_erase_data(spi_inst_t *spi, const uint8_t cs_pin, uint32_t address){
    uint8_t cmd[4] ={
            0x20,
            address >> 16,
            address >> 8,
            address
    };

    flash_write_enable(spi, cs_pin);
    SPI_write_blocking(spi,cs_pin,cmd, 4 );
    flash_wait_for_done(spi, cs_pin);
    return flash_erase_succeeded(spi, cs_pin);

}
int flash_write_page(spi_inst_t *spi, const uint8_t cs_pin, uint32_t address, uint8_t *data, uint16_t page_size ){
    uint8_t cmd[4] = {
            0x02,
            address >> 16,
            address >> 8,
            address
    };
    flash_write_enable(spi, cs_pin);
    SPI_write_blocking(spi, cs_pin, cmd, 4);
    int data_wrote= SPI_write_blocking(spi,cs_pin, data, page_size);
    flash_wait_for_done(spi, cs_pin);
    return data_wrote;
}

