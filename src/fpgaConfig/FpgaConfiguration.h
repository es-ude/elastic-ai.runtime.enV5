#ifndef ENV5_FPGA_CONFIGURATION_HEADER
#define ENV5_FPGA_CONFIGURATION_HEADER

#include <stdio.h>
enum {
    SEND,
    RECEIVE
  };typedef uint8_t data_direction_t;

void fpgaConfigurationFlashConfiguration();
void fpgaConfigurationVerifyConfiguration();



#endif /* ENV5_FPGA_CONFIGURATION_HEADER */
