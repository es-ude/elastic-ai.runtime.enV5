#ifndef ENV5_REGISTERABSTRACTION_H
#define ENV5_REGISTERABSTRACTION_H

#endif // ENV5_REGISTERABSTRACTION_H

#include "stdint.h"
#include "stdbool.h"

uint32_t rightShift(uint32_t input, uint8_t num);
uint32_t leftShift(uint32_t input, uint8_t num);
uint32_t rightRotate(uint32_t input, uint8_t num);
uint32_t leftRotate(uint32_t input, uint8_t num);
uint32_t bitwiseXor(uint32_t inputOne, uint32_t inputTwo);
uint32_t bitwiseNot(uint32_t input);
uint32_t smallSigmaZero(uint32_t input);
uint32_t smallSigmaOne(uint32_t input);