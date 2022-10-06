//
// Created by Natalie Maman on 02.06.22.
//

#include <stdint.h>

#include "FpgaConfigDatahandler.h"

uint8_t readValueNum = 0;

void setAddress(uint32_t addr) {
    address = addr;
}
void setConfigSize(uint32_t conf_size) {
    config_size = conf_size;
}
uint16_t readData(uint8_t *block, uint16_t buffer_length) {
    readValueNum = 0;
    for (uint16_t i = 0; i < buffer_length; i++) {
        block[i] = i;
    }
    return 0;
}
void readValue(uint32_t *destination) {
    if (readValueNum == 0) {
        *destination = address;
    } else if (readValueNum == 1) {
        *destination = config_size;
    }
    readValueNum++;
}