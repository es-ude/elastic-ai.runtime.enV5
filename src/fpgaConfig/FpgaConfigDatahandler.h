#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_HEADER

#include <stdint.h>

uint16_t fpgaConfigHandlerReceiveData(uint8_t *block, uint16_t bufferLength);

void fpgaConfigHandlerReadValue(uint32_t *destination);

void fpgaConfigHandlerSendData(char *data);
#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_HEADER */
