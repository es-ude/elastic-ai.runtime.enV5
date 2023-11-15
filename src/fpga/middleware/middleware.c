#define SOURCE_FILE "MIDDLEWARE"

#include "middleware.h"
#include "Gpio.h"
#include "Qxi.h"
#include "enV5HwController.h"

#include <stdbool.h>
#include <stdint.h>

#define ADDR_LEDS 0x0003
#define ADDR_USER_LOGIC_RESET 0x0004
#define ADDR_MULTI_BOOT 0x0005
#define ADDR_DESIGN_ID 0x08D0
#define ADDR_USER_LOGIC_OFFSET 0x0100

void middlewareInit() {
    qxiInit();
}

void middlewareDeinit() {
    qxiDeinit();
}

void middlewareConfigureFpga(uint32_t address) {
    uint8_t addr_arr[3];
    addr_arr[2] = (uint8_t)((0x00ff0000 & address) >> 16);
    addr_arr[1] = (uint8_t)((0x0000ff00 & address) >> 8);
    addr_arr[0] = (uint8_t)((0x000000ff & address));

    qxiWriteBlocking(ADDR_MULTI_BOOT, addr_arr, 3);
}

void middlewareSetFpgaLeds(uint8_t leds) {
    uint8_t write_data[1];
    write_data[0] = leds;

    qxiWriteBlocking(ADDR_LEDS, write_data, 1);
}

uint8_t middlewareGetLeds(void) {
    uint8_t read_data[1];

    qxiReadBlocking(ADDR_LEDS, read_data, 1);

    return (0x0f & read_data[0]);
}

void middlewareUserlogicEnable(void) {
    uint8_t addr_arr[1];
    addr_arr[0] = 0x00; // set user_logic_reset to 0 => enable

    qxiWriteBlocking(ADDR_USER_LOGIC_RESET, addr_arr, 1);
}

void middlewareUserlogicDisable(void) {
    uint8_t addr_arr[1];
    addr_arr[0] = 0x01; // keep reseting the userlogic

    qxiWriteBlocking(ADDR_USER_LOGIC_RESET, addr_arr, 1);
}

uint8_t middlewareGetDesignId(void) {
    uint8_t read_data[1];

    qxiReadBlocking(ADDR_DESIGN_ID, read_data, 1);

    return read_data[0];
}

void middlewareWriteBlocking(uint32_t address, uint8_t *data, uint16_t len) {
    qxiWriteBlocking(ADDR_USER_LOGIC_OFFSET + address, data, len);
}

uint8_t middlewareReadBlocking(uint32_t address, uint8_t *data, uint16_t len) {
    qxiReadBlocking(ADDR_USER_LOGIC_OFFSET + address, data, len);
}

bool middlewareUserlogicGetBusyStatus(void) {
    return gpioGetPin(FPGA_BUSY_PIN);
}