//****************************************************************************
// Copyright 2021 Richard Hulme
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Flashloader for the RP2040

#ifndef __FLASHLOADER_INCL__
#define __FLASHLOADER_INCL__

#include <stdint.h>

#define APPLICATION_NAME_MAX_LENGTH 40

static const uint32_t FLASH_MAGIC1 = 0x00000005; // Randomly picked numbers
static const uint32_t FLASH_MAGIC2 = 0x00000001;

static const uint32_t FLASH_APP_UPDATED = 0x01234567; // App has been updated

typedef struct
//    __packed __aligned(4)
{
    uint32_t magic1;
    uint32_t magic2;
    uint32_t length;
    uint32_t crc32;
    uint8_t sectors;
    char name[APPLICATION_NAME_MAX_LENGTH];
    uint32_t isProgram;
    uint8_t  data[];
}tFlashHeader;

#endif // __FLASHLOADER_INCL__
