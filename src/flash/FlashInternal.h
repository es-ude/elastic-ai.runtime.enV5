#ifndef ENV5_FLASH_INTERNAL_H
#define ENV5_FLASH_INTERNAL_H

void flashWriteEnable(void);

uint8_t flashReadStatusReg(void);

flashEraseErrorCode_t flashEraseErrorOccurred(void);

void flashWaitForDone(void);

#endif /* ENV5_FLASH_INTERNAL_H */
