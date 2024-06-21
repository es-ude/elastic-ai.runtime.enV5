#ifndef ENV5_FLASH_INTERNAL_HEADER
#define ENV5_FLASH_INTERNAL_HEADER

#include "FlashTypedefs.h"

/*!
 * \brief flash enable write has to be sent before every write/erase operation
 *        to enable modifications to the data
 */
static void flashEnableWrite(spiConfiguration_t *spiConfig);

static flashErrorCode_t flashEraseErrorOccurred(flashConfiguration_t *config);

static void flashWaitForDone(flashConfiguration_t *config);

#endif /* ENV5_FLASH_INTERNAL_HEADER */
