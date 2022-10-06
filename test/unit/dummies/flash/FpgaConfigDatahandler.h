//
// Created by Natalie Maman on 02.06.22.
//

#ifndef ENV5_FPGACONFIGDATAHANDLER_H
#define ENV5_CONFIG_DATA_HANDLER_H
#include <stdint.h>
static uint32_t address;
static uint32_t config_size;

void setAddress(uint32_t addr);
void setConfigSize(uint32_t conf_size);
uint16_t readData(uint8_t *block, uint16_t buffer_length);
void readValue(uint32_t *destination);
#endif // ENV5_FPGACONFIGDATAHANDLER_H
