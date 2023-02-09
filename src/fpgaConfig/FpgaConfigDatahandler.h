#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_HEADER

#include <stdint.h>

uint16_t fpgaConfigHandlerReceiveData(uint8_t *block, uint16_t bufferLength);

void fpgaConfigHandlerReadValue(uint32_t *destination);

void fpgaConfigHandlerSendData(uint8_t *data, uint16_t length);
void fpgaConfigHandlerSendAck();
void fpgaConfigHandlerSendDataAck(uint32_t *data);

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_HEADER */
