//
// Created by chao on 26/12/22.
//

#ifndef MY_PROJECT_MIDDLEWARE_H
#define MY_PROJECT_MIDDLEWARE_H
#include "qxi.h"

#define ADDR_MULTI_BOOT 0x0005
#define ADDR_LEDS 0x0003
#define ADDR_DESIGN_ID 2256
#define ADDR_USER_LOGIC_OFFSET 0x0100

void middleware_init();
void middleware_deinit();
void middleware_configure_fpga(uint32_t address);
void middleware_set_fpga_leds(uint8_t leds);
uint8_t middleware_get_leds(void);
uint8_t middleware_get_design_id(void);
void middleware_write_blocking(uint32_t address, uint8_t* data, uint16_t len);
uint8_t middleware_read_blocking(uint32_t address, uint8_t* data, uint16_t len);
#endif //MY_PROJECT_MIDDLEWARE_H
