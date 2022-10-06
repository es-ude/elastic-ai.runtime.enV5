#ifndef ENV5_FPGA_CONFIGURATION_INTERNAL_HEADER
#define ENV5_FPGA_CONFIGURATION_INTERNAL_HEADER

#include <stdint.h>

void fpgaConfigurationInternalPrintBuffer(uint8_t *buffer, uint16_t length);
void fpgaConfigurationInternalDebugPrintFlashAfterErase(uint8_t eraseStatus, uint16_t blockCounter,
                                                        uint32_t blockAddress);
void fpgaConfigurationInternalEraseSectors(void);
void fpgaConfigurationInternalFillBufferWithDebugData(uint8_t *buffer, uint16_t buffer_length);

#endif /* ENV5_FPGA_CONFIGURATION_INTERNAL_HEADER */
