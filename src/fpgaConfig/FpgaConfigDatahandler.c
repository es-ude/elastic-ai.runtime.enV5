#include "FpgaConfigDatahandler.h"
#include <pico/stdio.h>
#include <stdint.h>

uint16_t fpgaConfigHandlerReadData(uint8_t *block, uint16_t bufferLength) {
    uint16_t bufferIndex = 0;
    while (bufferIndex < bufferLength) {

        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            block[bufferIndex++] = (c);
        }
    }
    return bufferIndex;
}

void fpgaConfigHandlerReadValue(uint32_t *destination) {
    fpgaConfigHandlerReadData((uint8_t *)destination, sizeof(uint32_t));
}