#ifndef ENV5_OTA_H
#define ENV5_OTA_H

#include "FlashLoader.h"

typedef struct {
    const tFlashHeader *header;
    uint32_t totalLength;
    uint32_t eraseLength;
} flashParameter;

// Buffer to hold the incoming data before flashing
static union
{
    tFlashHeader header;
    uint8_t      buffer[sizeof(tFlashHeader) + 65536];
} flashbuf;

// Intel HEX record
typedef struct
{
    uint8_t  count;
    uint16_t addr;
    uint8_t  type;
    uint8_t  data[256];
}tRecord;

// Intel HEX record types
static const uint8_t TYPE_DATA      = 0x00;
static const uint8_t TYPE_EOF       = 0x01;
static const uint8_t TYPE_EXTSEG    = 0x02;
static const uint8_t TYPE_STARTSEG  = 0x03;
static const uint8_t TYPE_EXTLIN    = 0x04;
static const uint8_t TYPE_STARTLIN  = 0x05;

_Noreturn void loadHexHTTP(char *ip);

_Noreturn void flashHex(void);

#endif // ENV5_OTA_H
