//
// Created by Natalie Maman on 01.06.22.
//
#include <stdint.h>
#include <pico/stdio.h>
#include "config_data_handler.h"
#include "pico/printf.h"

uint16_t readData(uint8_t *block, uint16_t buffer_length) {
    uint16_t buffer_index = 0;
    while (buffer_index<buffer_length) {

        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            block[buffer_index++] = (c);
        }
    }
    return buffer_index;
}

void readValue(uint32_t *destination) {
   readData((uint8_t *) destination, sizeof(uint32_t));

}