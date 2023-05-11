
#ifndef ENV5_FLASHTYPEDEFS_H
#define ENV5_FLASHTYPEDEFS_H

#include <stdint.h>

#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 65536

enum {
    FLASH_ERASE_NO_ERROR = 0x00,
    FLASH_ERASE_ERROR = 0x01,

};
typedef uint8_t flashEraseErrorCode_t;
#endif // ENV5_FLASHTYPEDEFS_H
