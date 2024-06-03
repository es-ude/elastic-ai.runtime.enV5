#ifndef ENV5_FLASH_INTERNAL_HEADER
#define ENV5_FLASH_INTERNAL_HEADER

#include "FlashTypedefs.h"

/*!
 * \brief flash enable write has to be sent before every write/erase operation
 *        to enable modifications to the data
 */
static void flashEnableWrite(void);

static flashErrorCode_t flashEraseErrorOccurred(void);

static void flashWaitForDone(void);

#endif /* ENV5_FLASH_INTERNAL_HEADER */
