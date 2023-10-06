#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER

#include <stdint.h>

#include "HTTP.h"
#include "include/FpgaConfigurationHandler.h"

/* region HTTP */

static char *fpgaConfigurationHandlerGenerateUrl(char *baseUrl, size_t page);
static void fpgaConfigurationHandlerFreeUrl(char *url);

/* endregion HTTP */

/* region USB */

static void fpgaConfigurationHandlerWaitForStartRequest();
static uint32_t fpgaConfigurationHandlerGetFileLength();
static void fpgaConfigurationHandlerGetChunks(uint32_t totalLength, uint32_t startAddress);

/* endregion USB */

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER */
