#ifndef ENV5_FPGA_CONFIGURATION_INTERNAL_HEADER
#define ENV5_FPGA_CONFIGURATION_INTERNAL_HEADER

#include <stdint.h>

void fpgaConfigurationInternalPrintBuffer(uint8_t *buffer, uint16_t length);
void fpgaConfigurationInternalDebugPrintFlashAfterErase(uint8_t eraseStatus, uint16_t blockCounter,
                                                        uint32_t blockAddress);
uint8_t fpgaConfigurationInternalEraseSectors(uint32_t fpgaConfigurationConfigAddress,uint32_t fpgaConfigurationConfigSize);
void fpgaConfigurationInternalFillBufferWithDebugData(uint8_t *buffer, uint16_t buffer_length);
static void fpgaConfigurationInternalGetBitfileWriteArguments(uint32_t *flashAddress, uint32_t *sizeToReceive);

void fpgaConfigurationInternalPipeConfigToAndFromFlash( void (*fun_ptr) (uint32_t, uint8_t*, uint16_t),
uint8_t * configurationBuffer,
        uint32_t flashAddress, uint32_t configSize);
void fpgaConfigurationInternalReceiveAndWriteDataOnFlash(uint32_t currentAddress, uint8_t* configurationBuffer, uint16_t blockSize);
void fpgaConfigurationInternalReadDataOnFlash(uint32_t currentAddress, uint8_t* configurationBuffer, uint16_t blockSize);
#endif /* ENV5_FPGA_CONFIGURATION_INTERNAL_HEADER */
