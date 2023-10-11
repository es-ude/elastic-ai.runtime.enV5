#define SOURCE_FILE "QXI"

#include "middleware.h"
#include "qxi.h"
#include <stdbool.h>
#include <stdint.h>

void middleware_init() {
    qxi_init();
}

void middleware_deinit() {
    qxi_deinit();
}

void middleware_configure_fpga(uint32_t address) {
    uint8_t addr_arr[3];
    addr_arr[2] = (uint8_t)((0x00ff0000 & address) >> 16);
    addr_arr[1] = (uint8_t)((0x0000ff00 & address) >> 8);
    addr_arr[0] = (uint8_t)((0x000000ff & address));

    qxi_write_blocking(ADDR_MULTI_BOOT, addr_arr, 3);
}

void middleware_set_fpga_leds(uint8_t leds) {
    uint8_t write_data[1];
    write_data[0] = leds;

    qxi_write_blocking(ADDR_LEDS, write_data, 1);
}

uint8_t middleware_get_leds(void) {
    uint8_t read_data[1];

    qxi_read_blocking(ADDR_LEDS, read_data, 1);

    return (0x0f & read_data[0]);
}

void middleware_userlogic_enable(void) {
    uint8_t addr_arr[1];
    addr_arr[0] = 0x00; // set user_logic_reset to 0 => enable

    qxi_write_blocking(ADDR_USER_LOGIC_RESET, addr_arr, 1);
}

void middleware_userlogic_disable(void) {
    uint8_t addr_arr[1];
    addr_arr[0] = 0x01; // keep reseting the userlogic

    qxi_write_blocking(ADDR_USER_LOGIC_RESET, addr_arr, 1);
}

uint8_t middleware_get_design_id(void) {
    uint8_t read_data[1];

    qxi_read_blocking(ADDR_DESIGN_ID, read_data, 1);

    return read_data[0];
}

void middleware_write_blocking(uint32_t address, uint8_t *data, uint16_t len) {
    qxi_write_blocking(ADDR_USER_LOGIC_OFFSET + address, data, len);
}

uint8_t middleware_read_blocking(uint32_t address, uint8_t *data, uint16_t len) {
    qxi_read_blocking(ADDR_USER_LOGIC_OFFSET + address, data, len);
}

bool middleware_userlogic_get_busy_status(void) {
    return false; // TODO: fix this
}