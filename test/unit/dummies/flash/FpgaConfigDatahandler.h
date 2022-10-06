#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_HEADER

#include <stdint.h>

static uint32_t address;
static uint32_t config_size;

void fpgaConfigHandlerSetAddress(uint32_t addr);
void fpgaConfigHandlerSetConfigSize(uint32_t conf_size);
uint16_t fpgaConfigHandlerReadData(uint8_t *block, uint16_t bufferLength);
void fpgaConfigHandlerReadValue(uint32_t *destination);

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_HEADER */
