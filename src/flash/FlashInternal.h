#ifndef INCLUDE_ENV5_FLASH_INTERNAL_H
#warning Internal header file "FlashInternal.h" included from elsewhere
#undef INCLUDE_ENV5_FLASH_INTERNAL_H
#endif

#ifndef ENV5_FLASH_INTERNAL_H
#define ENV5_FLASH_INTERNAL_H

void flashWriteEnable(void);

uint8_t flashReadStatusReg(void);

uint8_t flashEraseErrorOccurred(void);

void flashWaitForDone(void);

#endif /* ENV5_FLASH_INTERNAL_H */
