#ifndef ENV5_FLASH_TYPEDEFS
#define ENV5_FLASH_TYPEDEFS

#include <stdint.h>
#include "SpiTypedefs.h"

typedef struct flashConfiguration {
    spiConfiguration_t *flashSpiConfiguration;
    uint32_t flashBytesPerSector;
    uint32_t flashBytesPerPage;
} flashConfiguration_t;


typedef enum flashErrorCode {
    FLASH_NO_ERROR = 0x00,
    FLASH_ERASE_ERROR = 0x01,
} flashErrorCode_t;

typedef enum commands {
    FLASH_READ_ID = 0x9F,
    FLASH_READ = 0x03,
    FLASH_ERASE_SECTOR = 0xD8,
    FLASH_BULK_ERASE = 0x60,
    FLASH_WRITE_PAGE = 0x02,
    FLASH_ENABLE_WRITE = 0x06,
    FLASH_STATUS_REGISTER = 0x05
} commands_t;

#endif // ENV5_FLASH_TYPEDEFS
