#ifndef ENV5_FPGACONFIGDATAHANDLER_H
#define ENV5_FPGACONFIGDATAHANDLER_H
#include <stdint.h>

uint16_t readData(uint8_t *block, uint16_t buffer_length);
void readValue(uint32_t *destination);
#endif // ENV5_FPGACONFIGDATAHANDLER_H
