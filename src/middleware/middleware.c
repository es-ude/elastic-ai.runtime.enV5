//
// Created by chao on 26/12/22.
//

#include "middleware.h"
#include "flash/Flash.h"

void middleware_init()
{
    qxi_init();
}

void middleware_deinit()
{
    qxi_deinit();
}


void middleware_configure_fpga(uint32_t address)
{
    uint8_t addr_arr[3];
    addr_arr[2] = (uint8_t)((0x00ff0000&address)>>16);
    addr_arr[1] = (uint8_t)((0x0000ff00&address)>>8);
    addr_arr[0] = (uint8_t)((0x000000ff&address));

    qxi_write_blocking(ADDR_MULTI_BOOT, addr_arr, 3);
}

void middleware_set_fpga_leds(uint8_t leds)
{
    uint8_t write_data[1];
    write_data[0] = leds;

    qxi_write_blocking(ADDR_LEDS, write_data, 1);
}

uint8_t middleware_get_leds(void)
{
    uint8_t read_data[1];

    qxi_read_blocking(ADDR_LEDS, read_data, 1);

    return (0x0f&read_data[0]);
}

uint8_t middleware_get_design_id(void)
{
    uint8_t read_data[1];

    qxi_read_blocking(ADDR_DESIGN_ID, read_data, 1);

    return read_data[0];
}


void middleware_write_blocking(uint32_t address, uint8_t* data, uint16_t len)
{
    qxi_write_blocking(ADDR_USER_LOGIC_OFFSET+address, data, len);
}

uint8_t middleware_read_blocking(uint32_t address, uint8_t* data, uint16_t len)
{
    qxi_read_blocking(ADDR_USER_LOGIC_OFFSET+address, data, len);
}

