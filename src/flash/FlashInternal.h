#ifndef ENV5_FLASH_INTERNAL_H
#define ENV5_FLASH_INTERNAL_H

#include <stdint.h>

void flashWriteEnable(void);

uint8_t flashReadStatusReg(void);

flashEraseErrorCode_t flashEraseErrorOccurred(void);

flashWriteError_t flashWritePage(uint32_t address, uint8_t *data, uint16_t data_size);

void flashWaitForDone(void);

#endif /* ENV5_FLASH_INTERNAL_H */
