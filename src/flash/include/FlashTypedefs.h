#ifndef ENV5_FLASH_TYPEDEFS
#define ENV5_FLASH_TYPEDEFS

#include "SpiTypedefs.h"
#include <stdint.h>

typedef struct flashConfiguration {
    spiConfiguration_t *spiConfiguration;
    size_t size;
    size_t bytesPerSector;
    size_t bytesPerPage;
    size_t numberOfSectors;
} flashConfiguration_t;

typedef enum flashErrorCode {
    FLASH_NO_ERROR = 0x00,
    FLASH_ERASE_ERROR = 0x01,
} flashErrorCode_t;

typedef enum commands {
    FLASH_READ = 0x03,
    FLASH_ERASE_SECTOR = 0xD8,
    FLASH_BULK_ERASE = 0x60,
    FLASH_WRITE_PAGE = 0x02,
    FLASH_ENABLE_WRITE = 0x06,
    FLASH_READ_ID = 0x9F, //!< Byte 0: Manufacturer ID; Byte 1-2: Device ID; Byte 3-...: CFI
    FLASH_READ_STATUS_REGISTER = 0x05,
    FLASH_READ_CONFIG_REGISTER = 0x35,
    FLASH_WRITE_CONFIG_REGISTER = 0x01, //!< Byte 0-1: Status Register; Byte 2-3: Config Register
} commands_t;

#endif // ENV5_FLASH_TYPEDEFS
