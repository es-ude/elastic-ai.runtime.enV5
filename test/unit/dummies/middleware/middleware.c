#define SOURCE_FILE "MIDDLEWARE"

#include <stdbool.h>

#include "middleware.h"

void middlewareInit() {}

void middlewareDeinit() {}

/* region MIDDLEWARE */

#define ADDR_LEDS 0x0003
#define ADDR_USER_LOGIC_RESET 0x0004
#define ADDR_MULTI_BOOT 0x0005

void middlewareConfigureFpga(uint32_t address) {}

void middlewareSetFpgaLeds(uint8_t leds) {}

uint8_t middlewareGetLeds(void) {}

void middlewareUserlogicEnable(void) {}

void middlewareUserlogicDisable(void) {}

/* endregion MIDDLEWARE */

/* region USER LOGIC */

//! has to be added to user logic address (see documentation)
#define USER_LOGIC_OFFSET 0x0100
#ifndef FPGA_BUSY_PIN
#define FPGA_BUSY_PIN 15
#endif

void middlewareWriteBlocking(uint32_t address, uint8_t *data, size_t length) {}

void middlewareReadBlocking(uint32_t address, uint8_t *data, size_t length) {}

bool middlewareUserlogicGetBusyStatus(void) {}

/* endregion USER LOGIC */
