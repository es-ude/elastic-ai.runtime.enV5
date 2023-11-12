#define SOURCE_FILE "FPGA-CONFIGRATION-HANDLER-BASE"

#include "include/FpgaConfigurationHandler.h"
#include "Flash.h"
#include "middleware.h"

void fpgaConfigurationHandlerInitialize() {}

fpgaConfigurationHandlerError_t fpgaConfigurationFlashFpga(uint32_t sectorID) {
    uint32_t startAddress = (sectorID - 1) * FLASH_BYTES_PER_SECTOR;

    middlewareConfigureFpga(startAddress);
    return FPGA_RECONFIG_NO_ERROR;
}
