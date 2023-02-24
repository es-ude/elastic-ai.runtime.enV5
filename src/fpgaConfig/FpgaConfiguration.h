#ifndef ENV5_FPGA_CONFIGURATION_HEADER
#define ENV5_FPGA_CONFIGURATION_HEADER

#include <stdio.h>
#include <stdint.h>
#include <FlashTypedefs.h>

void fpgaConfigurationFlashConfiguration();
void fpgaConfigurationVerifyConfiguration();

flashEraseErrorCode_t fpgaConfigurationEraseSectors(uint32_t fpgaConfigurationConfigAddress,
                                              uint32_t fpgaConfigurationConfigSize);


#endif /* ENV5_FPGA_CONFIGURATION_HEADER */
