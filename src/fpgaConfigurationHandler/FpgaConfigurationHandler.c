#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER-BASE"

#include "include/FpgaConfigurationHandler.h"
#include "middleware.h"

void fpgaConfigurationHandlerInitialize() {}

fpgaConfigurationHandlerError_t fpgaConfigurationFlashFpga(uint32_t startAddress) {
    middlewareConfigureFpga(startAddress);
    return FPGA_RECONFIG_NO_ERROR;
}
