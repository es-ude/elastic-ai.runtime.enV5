#define SOURCE_FILE "QXI"

/* NOTE:
 *   - Information about the correct register address can be found in the elastic-ai.creator
 *     repository: https://github.com/es-ude/elastic-ai.creator
 *   - Middleware: `elasticai/creator/vhdl/system_integrations/middleware`
 *   - Skeleton: `elasticai/creator/vhdl/system_integrations/skeleton`
 */

#include <stdint.h>

#include "include/middleware.h"
#include "qxi.h"

typedef enum fpgaRegister {
    ADDR_MULTI_BOOT = 0x0005,
    ADDR_LEDS = 0x0003,
    ADDR_DESIGN_ID = 2256,
    ADDR_USER_LOGIC_OFFSET = 0x0100
} fpgaRegister_t;

void middlewareConfigureFpga(uint32_t address) {
    uint8_t addrArray[3];
    addrArray[0] = 0xFF & address;
    addrArray[1] = 0xFF & (address >> 8);
    addrArray[2] = 0xFF & (address >> 16);

    qxiInit();
    qxiWriteBlocking(ADDR_MULTI_BOOT, addrArray, 3);
    qxiDeinit();
}

void middlewareWriteBlocking(uint32_t address, uint8_t *data, uint16_t length) {
    qxiInit();
    qxiWriteBlocking(ADDR_USER_LOGIC_OFFSET + address, data, length);
    qxiDeinit();
}
uint8_t middlewareReadBlocking(uint32_t address, uint8_t *data, uint16_t length) {
    qxiInit();
    qxiReadBlocking(ADDR_USER_LOGIC_OFFSET + address, data, length);
    qxiDeinit();
}

uint8_t middlewareGetDesignId(void) {
    uint8_t read_data[1];

    qxiInit();
    qxiReadBlocking(ADDR_DESIGN_ID, read_data, 1);
    qxiDeinit();

    return read_data[0];
}
void middleware_set_fpga_leds(uint8_t leds) {
    uint8_t write_data[1];
    write_data[0] = leds;

    qxiInit();
    qxiWriteBlocking(ADDR_LEDS, write_data, 1);
    qxiDeinit();
}
uint8_t middleware_get_leds(void) {
    uint8_t read_data[1];

    qxiInit();
    qxiReadBlocking(ADDR_LEDS, read_data, 1);
    qxiDeinit();

    return (0x0F & read_data[0]);
}
