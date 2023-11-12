#define SOURCE_FILE "MIDDLEWARE"

#include "include/middleware.h"

#include <stdbool.h>
#include <stdint.h>

#define ADDR_LEDS 0x0003
#define ADDR_USER_LOGIC_RESET 0x0004
#define ADDR_MULTI_BOOT 0x0005
#define ADDR_DESIGN_ID 0x08D0
#define ADDR_USER_LOGIC_OFFSET 0x0100

void middlewareInit() {}

void middlewareDeinit() {}

void middlewareConfigureFpga(uint32_t address) {}

void middlewareSetFpgaLeds(uint8_t leds) {}

uint8_t middlewareGetLeds(void) {}

void middlewareUserlogicEnable(void) {}

void middlewareUserlogicDisable(void) {}

uint8_t middlewareGetDesignId(void) {}

void middlewareWriteBlocking(uint32_t address, uint8_t *data, uint16_t len) {}

uint8_t middlewareReadBlocking(uint32_t address, uint8_t *data, uint16_t len) {}

bool middlewareUserlogicGetBusyStatus(void) {}