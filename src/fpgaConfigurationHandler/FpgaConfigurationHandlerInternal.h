#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER

#include <stdint.h>

#include "HTTP.h"
#include "include/FpgaConfigurationHandler.h"

static char *fpgaConfigurationHandlerGenerateUrl(char *baseUrl, size_t page);
static void fpgaConfigurationHandlerFreeUrl(char *url);

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER */
