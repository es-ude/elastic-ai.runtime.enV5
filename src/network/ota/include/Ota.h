#ifndef ENV5_OTA_H
#define ENV5_OTA_H

#include "FlashLoader.h"

#define PROGRAM_IS_PRESENT 0x0A0A0A0A

typedef struct {
    const tFlashHeader *header;
    uint32_t totalLength;
    uint32_t eraseLength;
    uint32_t flashOffset;
} flashParameter;

// Buffer to hold the incoming data before flashing
static union
{
    tFlashHeader header;
    uint8_t      buffer[sizeof(tFlashHeader) + 2 * 65536];
} flashbuf;

// Intel HEX record
typedef struct
{
    uint8_t  count;
    uint16_t addr;
    uint8_t  type;
    uint8_t  data[256];
}tRecord;

typedef enum APPLICATION_POSITION {
    FIRST = 1,
    SECOND = 2,
    THIRD = 3,
    FOURTH = 4
} applicationFlashPosition;

// Intel HEX record types
static const uint8_t TYPE_DATA      = 0x00;
static const uint8_t TYPE_EOF       = 0x01;
static const uint8_t TYPE_EXTSEG    = 0x02;
static const uint8_t TYPE_STARTSEG  = 0x03;
static const uint8_t TYPE_EXTLIN    = 0x04;
static const uint8_t TYPE_STARTLIN  = 0x05;

void loadHexHTTP(char *ip, applicationFlashPosition pos, char *name);

_Noreturn void restartToApplication(applicationFlashPosition pos);

#endif // ENV5_OTA_H
