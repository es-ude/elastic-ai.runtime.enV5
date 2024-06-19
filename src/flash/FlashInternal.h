#ifndef ENV5_FLASH_INTERNAL_HEADER
#define ENV5_FLASH_INTERNAL_HEADER

#include "FlashTypedefs.h"

/*!
 * \brief flash enable write has to be sent before every write/erase operation
 *        to enable modifications to the data
 */
static void flashEnableWrite(spiConfiguration_t *flashSpiConfiguration);

static uint8_t flashReadStatusRegister(spiConfiguration_t *flashSpiConfiguration);

static flashErrorCode_t flashEraseErrorOccurred(spiConfiguration_t *flashSpiConfiguration);

static void flashWaitForDone(spiConfiguration_t *flashSpiConfiguration);

#endif /* ENV5_FLASH_INTERNAL_HEADER */
