#ifndef ENV5_FLASH_INTERNAL_HEADER
#define ENV5_FLASH_INTERNAL_HEADER

#include "include/FlashTypedefs.h"

static void flashEnableWrite(void);

static uint8_t flashReadStatusRegister(void);

static flashErrorCode_t flashEraseErrorOccurred(void);

static void flashWaitForDone(void);

#endif /* ENV5_FLASH_INTERNAL_HEADER */
