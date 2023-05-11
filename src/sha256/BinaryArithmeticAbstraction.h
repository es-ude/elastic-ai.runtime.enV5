#ifndef ENV5_REGISTERABSTRACTION_H
#define ENV5_REGISTERABSTRACTION_H

#endif // ENV5_REGISTERABSTRACTION_H

#include "stdint.h"
#include <stdbool.h>

uint32_t rightShift(uint32_t input, uint8_t num);
uint32_t leftShift(uint32_t input, uint8_t num);
uint32_t rightRotate(uint32_t input, uint8_t num);
uint32_t leftRotate(uint32_t input, uint8_t num);
bool xor(bool inputOne, bool inputTwo);