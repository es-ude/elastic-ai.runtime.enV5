#ifndef ENV5_FPGA_CONFIGURATION_HANDLER_HEADER
#define ENV5_FPGA_CONFIGURATION_HANDLER_HEADER

#include <stddef.h>
#include <stdint.h>

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
 * @param startAddress address of the byte where the configuration starts
 *                     -> this address must be the start address of a sector
 * @return 0 if no error occurred
 */
fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaHttp(char *baseUrl, size_t length,
                                                     uint32_t startAddress);

/*! \brief download configuration to flash (USB)
 *  \ingroup FPGA
 *
 * @param startAddress address of the byte where the configuration starts
 *                     -> this address must be the start address of a sector
 * @return 0 if no error occurred
 */
fpgaConfigurationHandlerError_t
fpgaConfigurationHandlerDownloadConfigurationViaUsb(uint32_t startAddress);

// fpgaConfigurationHandlerError_t fpgaConfigurationHandlerVerifyConfiguration();
//  -> requires hash computation

/*! \brief reconfigures the FPGA with the given configuration
 *  \ingroup FPGA
 *
 * @param startAddress address of the byte where the configuration starts
 * @return 0 if no error occurred
 */
fpgaConfigurationHandlerError_t fpgaConfigurationFlashFpga(uint32_t startAddress);

#endif /* ENV5_FPGA_CONFIGURATION_HANDLER_HEADER */
