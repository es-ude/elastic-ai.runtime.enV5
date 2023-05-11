#ifndef ENV5_FPGA_CONFIGURATION_HEADER
#define ENV5_FPGA_CONFIGURATION_HEADER

#include <FlashTypedefs.h>
#include <stdint.h>
#include <stdio.h>
enum {
    CONFIG_NO_ERROR = 0x00,
    CONFIG_ERASE_ERROR = 0x01,

};
typedef uint8_t configErrorCode_t;
void fpgaConfigurationFlashConfiguration();
void fpgaConfigurationVerifyConfiguration();

flashEraseErrorCode_t fpgaConfigurationEraseSectors(uint32_t fpgaConfigurationConfigAddress,
                                                    uint32_t fpgaConfigurationConfigSize);

#endif /* ENV5_FPGA_CONFIGURATION_HEADER */
