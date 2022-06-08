//
// Created by Natalie Maman on 01.06.22.
//

#ifndef ENV5_CONFIG_DATA_HANDLER_H
#define ENV5_CONFIG_DATA_HANDLER_H
#include <stdint.h>

uint16_t readData(uint8_t *block, uint16_t buffer_length);
void readValue(uint32_t *destination);
#endif //ENV5_CONFIG_DATA_HANDLER_H


