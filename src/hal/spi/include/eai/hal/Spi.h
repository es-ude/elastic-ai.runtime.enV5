#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER

#include <stddef.h>
#include <stdint.h>

#include "eai/hal/SpiTypedefs.h"

/*!
 * @brief initializes the ports for the SPI interfaces
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param spiConfiguration[in]: struct that contains the SPI configuration
 */
void spiInit(spiConfiguration_t *spiConfiguration);

/*!
 * @brief terminates the ports for the SPI interfaces
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param spiConfiguration[in]: struct that contains the SPI configuration
 */
void spiDeinit(spiConfiguration_t *spiConfiguration);

/*!
 * @brief sends command to selected device, which can be found in spiConfiguration
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param spiConfiguration[in]: struct that contains the SPI configuration
 * @param command[in]: pointer to data_t, command to be send to slave
 */
int spiWriteCommandBlocking(spiConfiguration_t *spiConfiguration, data_t *command);

/*!
 * @brief sends a byte array to slave
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param spiConfiguration[in]: struct that contains the SPI configuration
 * @param command[in]: pointer to data_t, command to be send
 * @param data[in]: pointer to data_t, data to be send
 */
int spiWriteCommandAndDataBlocking(spiConfiguration_t *spiConfiguration, data_t *command,
                                   data_t *data);

/*!
 * @brief reads a byte array
 *
 * @IMPORTANT We highly recommend using the "enV5_hw_configuration_rev_[x]" -library
 *
 * @param spiConfiguration[in]: struct that contains the SPI configuration
 * @param command[in]: pointer to data_t, command to be send
 * @param data[out]: pointer to data_t, data that stores the received data
 */
int spiWriteCommandAndReadBlocking(spiConfiguration_t *spiConfiguration, data_t *command,
                                   data_t *data);

#endif /* ENV5_SPI_HEADER */
