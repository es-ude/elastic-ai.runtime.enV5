#ifndef ENV5_FLASH_UNITTEST_HEADER
#define ENV5_FLASH_UNITTEST_HEADER

// #include "eai/flash/Flash.h"
// #include "eai/flash/FlashTypedefs.h"

#include <memory.h>
#include <stdbool.h>
#include <stdint.h>

extern uint8_t *flashStorage;
extern size_t flashStorageLength;
extern bool flashInitCalled;

void flashSetUpDummyStorage(size_t lengthInBytes);
void flashRemoveDummyStorage(void);

#endif /* ENV5_FLASH_UNITTEST_HEADER */
