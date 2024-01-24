#define SOURCE_FILE "MIDDLEWARE-V1"

#include <stdbool.h>
#include <stdint.h>

#include "Gpio.h"
#include "Qxi.h"
#include "enV5HwController.h"
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
#define ADDR_USER_LOGIC_OFFSET 0x0100
#define ADDR_DESIGN_ID 0x07D0
#define ADDR_CONTROL_COMPUTATION 0x0064
#ifndef FPGA_BUSY_PIN
#define FPGA_BUSY_PIN 15
#endif

uint8_t middlewareGetDesignId(void) {
    uint8_t read_data[1];

    qxiReadBlocking(ADDR_DESIGN_ID + ADDR_USER_LOGIC_OFFSET, read_data, 1);

    return read_data[0];
}

void middlewareWriteBlocking(uint32_t address, uint8_t *data, size_t length) {
    qxiWriteBlocking(address + ADDR_USER_LOGIC_OFFSET, data, length);
}

uint8_t middlewareReadBlocking(uint32_t address, uint8_t *data, size_t length) {
    qxiReadBlocking(address + ADDR_USER_LOGIC_OFFSET, data, length);
}

void middlewareStartComputation(void) {
    uint8_t cmd[1] = {0x01};

    qxiWriteBlocking(ADDR_CONTROL_COMPUTATION + ADDR_USER_LOGIC_OFFSET, cmd, 1);
}

void middlewareStopComputation(void) {
    uint8_t cmd[1] = {0x00};

    qxiWriteBlocking(ADDR_CONTROL_COMPUTATION + ADDR_USER_LOGIC_OFFSET, cmd, 1);
}

bool middlewareUserlogicGetBusyStatus(void) {
    return gpioGetPin(FPGA_BUSY_PIN);
}

/* endregion USER LOGIC */
