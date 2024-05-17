#define SOURCE_FILE "MIDDLEWARE"

#include <stdbool.h>
#include <stdint.h>

#include "EnV5HwController.h"
#include "Gpio.h"
#include "Qxi.h"
#include "middleware.h"

void middlewareInit() {
    qxiInit();
}

void middlewareDeinit() {
    qxiDeinit();
}

/* region MIDDLEWARE */

#define ADDR_LEDS 0x0003
#define ADDR_USER_LOGIC_RESET 0x0004
#define ADDR_MULTI_BOOT 0x0005

void middlewareConfigureFpga(uint32_t address) {
    uint8_t configAddress[3] = {(uint8_t)(0x000000FF & address),
                                (uint8_t)((0x0000FF00 & address) >> 8),
                                (uint8_t)((0x00FF0000 & address) >> 16)};

    qxiWriteBlocking(ADDR_MULTI_BOOT, configAddress, 3);
}

void middlewareSetFpgaLeds(uint8_t leds) {
    uint8_t write_data[1] = {0x0F & leds};

    qxiWriteBlocking(ADDR_LEDS, write_data, 1);
}

uint8_t middlewareGetLeds(void) {
    uint8_t read_data[1];

    qxiReadBlocking(ADDR_LEDS, read_data, 1);

    return (0x0F & read_data[0]);
}

void middlewareUserlogicEnable(void) {
    // set user_logic_reset pin to 0 => enable
    uint8_t addr_arr[1] = {0x00};

    qxiWriteBlocking(ADDR_USER_LOGIC_RESET, addr_arr, 1);
}

void middlewareUserlogicDisable(void) {
    // set user_logic_reset pin to 1 => disable by continuously trigger reset
    uint8_t addr_arr[1] = {0x01};

    qxiWriteBlocking(ADDR_USER_LOGIC_RESET, addr_arr, 1);
}

/* endregion MIDDLEWARE */

/* region USER LOGIC */

//! has to be added to user logic address (see documentation)
#define USER_LOGIC_OFFSET 0x0100
#ifndef FPGA_BUSY_PIN
#define FPGA_BUSY_PIN 15
#endif

void middlewareWriteBlocking(uint32_t address, uint8_t *data, size_t length) {
    qxiWriteBlocking(address + USER_LOGIC_OFFSET, data, length);
}

void middlewareReadBlocking(uint32_t address, uint8_t *data, size_t length) {
    qxiReadBlocking(address + USER_LOGIC_OFFSET, data, length);
}

bool middlewareUserlogicGetBusyStatus(void) {
    return gpioGetPin(FPGA_BUSY_PIN);
}

/* endregion USER LOGIC */
