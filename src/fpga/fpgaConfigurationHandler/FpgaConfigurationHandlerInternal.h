#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER

#include <stdint.h>

#include "FpgaConfigurationHandler.h"
#include "HTTP.h"

/* region HTTP */

static char *fpgaConfigurationHandlerGenerateUrl(char *baseUrl, size_t page);
static void fpgaConfigurationHandlerFreeUrl(char *url);

/* endregion HTTP */

/* region USB */

static void fpgaConfigurationHandlerWaitForStartRequest();
static uint32_t fpgaConfigurationHandlerGetFileLength();
static void fpgaConfigurationHandlerGetChunks(flashConfiguration_t *flashConfiguration,
                                              uint32_t totalLength, uint32_t startAddress);

/* endregion USB */

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_INTERNAL_HEADER */
