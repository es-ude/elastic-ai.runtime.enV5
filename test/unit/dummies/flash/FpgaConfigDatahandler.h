#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_HEADER

#include "FlashTypedefs.h"
#include <stdint.h>

extern uint32_t address;
extern uint32_t config_size;
extern uint8_t dataSent[FLASH_PAGE_SIZE * 5];
extern uint8_t fpgaConfigHandlerNumWrittenBlock;

void fpgaConfigHandlerSetAddress(uint32_t addr);
void fpgaConfigHandlerSetConfigSize(uint32_t conf_size);

uint16_t fpgaConfigHandlerReceiveData(uint8_t *block, uint16_t bufferLength);
void fpgaConfigHandlerReadValue(uint32_t *destination);
void fpgaConfigHandlerSendData(uint8_t *data, uint16_t length);
void fpgaConfigHandlerSendAck();
void fpgaConfigHandlerSendDataAck(uint32_t *data);
#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_HEADER */
