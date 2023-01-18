#include "FpgaConfigDatahandler.h"
#include "pico/printf.h"
#include <pico/stdio.h>
#include <stdint.h>

uint16_t fpgaConfigHandlerReceiveData(uint8_t *block, uint16_t bufferLength) {
    uint16_t bufferIndex = 0;
    while (bufferIndex < bufferLength) {

        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            block[bufferIndex++] = c;
        }
    }
    return bufferIndex;
}

void fpgaConfigHandlerReadValue(uint32_t *destination) {
    fpgaConfigHandlerReceiveData((uint8_t *) destination, sizeof(uint32_t));
}

void fpgaConfigHandlerSendData(uint8_t *data, uint16_t length){
    for (uint32_t i = 0; i < length; i++) {
        printf("%u###", data[i]);
      }
      printf("\n");
}

void fpgaConfigHandlerSendAck(){
  printf("ack\n");
}