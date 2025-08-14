#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_HEADER

#include <stddef.h>
#include <stdint.h>

#include "eai/flash/FlashTypedefs.h"

typedef enum fpgaConfigurationHandlerError {
    FPGA_RECONFIG_NO_ERROR = 0x00,
    FPGA_RECONFIG_ERASE_ERROR,
    FPGA_RECONFIG_CONFIG_NOT_EXISTING,
    FPGA_RECONFIG_NOT_IMPLEMENTED,
    FPGA_RECONFIG_NETWORK_ERROR,
} fpgaConfigurationHandlerError_t;

/*! \brief initializes the configuration handler
 *  \ingroup FPGA
 *
 *  sets up a new list to store information about downloaded configurations
 */
void fpgaConfigurationHandlerInitialize();

/*! \brief download configuration to flash (HTTP)
 *  \ingroup FPGA
 *
 * @param baseUrl url that represents the base for the download url
 * @param length length of the configuration in Bytes
 * @param sectorID ID of the sector where the configuration starts
 * @return 0 if no error occurred
 */
fpgaConfigurationHandlerError_t fpgaConfigurationHandlerDownloadConfigurationViaHttp(
    flashConfiguration_t *flashConfiguration, char *baseUrl, size_t length, uint32_t sectorID);

/*! \brief download configuration to flash (USB)
 *  \ingroup FPGA
 *
 * @param sectorID ID of the sector where the configuration starts
 * @return 0 if no error occurred
 */
fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaUsb(flashConfiguration_t *flashConfiguration,
                                                    uint32_t sectorID);

// fpgaConfigurationHandlerError_t fpgaConfigurationHandlerVerifyConfiguration();
//  -> requires hash computation

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_HEADER */
