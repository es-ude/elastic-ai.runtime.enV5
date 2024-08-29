#define SOURCE_FILE "OTA"
//****************************************************************************
// Copyright 2021 Richard Hulme
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Demo application to test the flashloader.
// Listens on the default UART for an Intel hex file containing a new
// application.  This is stored in flash and the system is rebooted into the
// flashloader which overwrites the existing application with the new image
// and boots into it.
// Because the flashloader is not overwriting itself, it is power-fail safe.
//
// This code is for demonstration purposes.  There is not very much
// error-checking and attempts have been made to keep the final code size
// small (e.g. not using printf)
#include "Ota.h"
#include "CException.h"
#include "Common.h"
#include "EnV5HwController.h"
#include "FlashLoader.h"
#include "Gpio.h"
#include "HTTP.h"
#include "hardware/flash.h"
#include "hardware/structs/watchdog.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"
#include "pico/flash.h"
#include <malloc.h>
#include <string.h>
#include <sys/cdefs.h>

#define CHUNK_SIZE 1024

int currentChunk;
int charPos;
char chunkBuffer[CHUNK_SIZE];

//****************************************************************************
// Simple CRC32 (no reflection, no final XOR) implementation.
// This can be done with a lookup table or using the DMA sniffer too.
uint32_t crc32(const uint8_t *data, uint32_t len, uint32_t crc) {
    while(len--)
    {
        crc ^= (*data++ << 24);

        for(int bit = 0; bit < 8; bit++)
        {
            if(crc & (1L << 31))
                crc = (crc << 1) ^ 0x04C11DB7;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

//****************************************************************************
// Converts an ASCII hex character into its binary representation.
// The existing value is shifted across one nibble before the new value is
// stored in the lower nibble.
// Returns non-zero if the character could be converted
int hex2nibble(char c, uint8_t* value) {
    int success = 0;

    if(c >= '0' && c <= '9')
    {
        *value <<= 4;
        *value |= (uint8_t)(c - '0');
        success = 1;
    }
    else
    {
        c |= 32;
        if(c >= 'a' && c <= 'z')
        {
            *value <<= 4;
            *value |= (uint8_t)(c - 'a') + 10;
            success = 1;
        }
    }

    return success;
}

//****************************************************************************
// Converts two ASCII hex characters to an 8-bit binary value.
// Returns non-zero if valid hex characters were found
int parseHex(const char* str, uint8_t* value) {
    int success;

    *value = 0;
    success = hex2nibble(*str++, value) && hex2nibble(*str, value);

    return success;
}

//****************************************************************************
// Converts an Intel hex record in text form to a binary representation.
// Returns non-zero if the text could be parsed successfully
int processRecord(const char* line, tRecord* record) {
    int     success = 0;
    int     offset = 0;
    uint8_t value;
    uint8_t data[256 + 5]; // Max payload 256 bytes plus 5 for fields
    uint8_t checksum = 0;

    while(*line && (*line != ':'))
        line++;

    if(*line++ == ':')
    {
        while(parseHex(line, &value) && (offset < sizeof(data)))
        {
            data[offset++] = value;
            checksum += value;
            line += 2;
        }
    }

    // Checksum is two's-complement of the sum of the previous bytes so
    // final checksum should be zero if everything was OK.
    if((offset > 0) && (checksum == 0))
    {
        record->count = data[0];
        record->addr  = data[2] | (data[1] << 8);
        record->type  = data[3];
        memcpy(record->data, &data[4], data[0]);
        success = 1;
    } else {
        PRINT("CHECKSUM OF BLOCK NOT CORRECT!");
    }
    return success;
}

static void eraseAndProgramFlash(void *params) {
    const flashParameter *flashParams = (const flashParameter *)params;
    flash_range_erase(flashParams->flashOffset, flashParams->eraseLength);
    flash_range_program(flashParams->flashOffset, (uint8_t*)flashParams->header,
                        flashParams->totalLength);
}

void flashImage(tFlashHeader *header, uint32_t length, uint8_t pos, char name[APPLICATION_NAME_MAX_LENGTH]) {
    // Calculate length of header plus length of data
    uint32_t totalLength = sizeof(tFlashHeader) + length;

    // Round erase length up to next 4096 byte boundary
    uint32_t eraseLength = (totalLength + 4095) & 0xfffff000;
    uint32_t status;
    
    uint8_t sectors = length / 64 / 1024 + 1;

    if (pos + sectors > 16) {
        return;
    }
    
    header->magic1 = FLASH_MAGIC1;
    header->magic2 = FLASH_MAGIC2;
    header->length = length;
    header->sectors = sectors;
    header->isProgram = PROGRAM_IS_PRESENT;
    strncpy(header->name, name, APPLICATION_NAME_MAX_LENGTH - 1);
    header->crc32  = crc32(header->data, length, 0xffffffff);

    PRINT("Storing new image in flash");
    
    uint32_t flashImageOffset = (16 + pos) * 64 * 1024;
        
    PRINT("Offset: %lu", flashImageOffset);
    status = save_and_disable_interrupts();
    flashParameter par = {.eraseLength=eraseLength, .totalLength=totalLength, .header=header, .flashOffset=flashImageOffset};
    flash_safe_execute(eraseAndProgramFlash, &par, UINT32_MAX);
    restore_interrupts(status);
    
}

_Noreturn void restartToApplication(uint8_t pos) {
    PRINT("Rebooting into flash loader in 1 second");
    PRINT("To position %u", pos);
    // Set up watchdog scratch registers so that the flashloader knows
    // what to do after the reset
    watchdog_hw->scratch[0] = FLASH_MAGIC1;
    watchdog_hw->scratch[1] = XIP_BASE + (16 + pos) * 64 * 1024;
    
    watchdog_reboot(0x00000000, 0x00000000, 1000);

    // Wait for the reset
    while(true)
        tight_loop_contents();
}

char* getLine(char* buffer, char *ip) {
    char currentChar;
    char* ptr = buffer;
    do
    {
        if (charPos >= CHUNK_SIZE) {
            HttpResponse_t *response = NULL;
            char buf[100];
            snprintf(buf, 100, "%s?chunkNumber=%i", ip, currentChunk);

            uint8_t attempts = 0;
            while (true) {
                CEXCEPTION_T exception;
                Try {
                    HTTPGet(buf, &response);
                    strcpy(chunkBuffer, (const char *)response->response);
                    HTTPCleanResponseBuffer(response);
                    break;
                } Catch(exception) {
                    PRINT("Error in getting block %i over HTTP!", currentChunk);
                    attempts++;
                    if (attempts > 3) {
                        PRINT("Could not get application over HTTP!");
                        break;
                    }
                }
            }

            currentChunk++;
            charPos = 0;
        }
        currentChar = chunkBuffer[charPos];
        charPos++;

        if (currentChar == '\r')
            continue;

        if((currentChar != '\n'))
            *ptr++ = currentChar;
        else {
            *ptr++ = 0;
        }
    } while((currentChar != '\n'));
    return buffer;
}

tFlashHeader getApplicationHeader(uint8_t sector) {
    tFlashHeader* header = (tFlashHeader*)(XIP_BASE + (16 + sector) * 64 * 1024);
    if (header->isProgram== PROGRAM_IS_PRESENT)
        return *header;
    return ((tFlashHeader) {.name="NULL"});
}

char *getStoredApplications() {
    char *result = malloc(16 * (APPLICATION_NAME_MAX_LENGTH + 5));
    char * buf = malloc(APPLICATION_NAME_MAX_LENGTH + 5);
    strcpy(result,"");
    
    for (int i = 0; i < 16; i++) {
        tFlashHeader header = getApplicationHeader(i);
        for (int j = i + 1; j < i + header.sectors; j++) {
            if(getApplicationHeader(j).isProgram) {
                strcpy(header.name, "NULL");
            }
        }
        if (strcmp(header.name, "NULL") != 0) {
            sprintf(buf, "%i-%i:%s,", i, i + header.sectors, header.name);
            strcat(result, buf);
        }
    }
    if (strlen(result) == 0)
        strcat(result, "NULL");
    free(buf);
    return result;
}

void loadHexHTTP(char *ip, uint8_t sector, char name[APPLICATION_NAME_MAX_LENGTH]) {
    uint32_t      offset = 0;
    char          line[1024];
    uint32_t      count = 0;
    char addr[100];
    sprintf(addr, "%s%s", ip, name);
    currentChunk = 0;
    charPos = CHUNK_SIZE + 1;
    
    while (true) {
        tRecord rec;
        if(processRecord(getLine(line, addr), &rec)) {
            switch(rec.type) {
            case TYPE_DATA:
                memcpy(&flashbuf.header.data[offset], rec.data, rec.count);
                offset += rec.count;
                if((offset % 1024) == 0) {
                    gpioSetPin(25, offset/1024 % 2);
                }
                break;
            case TYPE_EOF:
                gpioSetPin(25, 0);
                flashImage(&flashbuf.header, offset, sector, name);
                return;
            case TYPE_EXTSEG:
            case TYPE_STARTSEG:
            case TYPE_STARTLIN:
            case TYPE_EXTLIN:
                break;

            default:
                break;
            }
            count++;
        }
    }
}
