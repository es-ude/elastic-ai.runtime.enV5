#ifndef ENV5_REGISTERABSTRACTION_H
#define ENV5_REGISTERABSTRACTION_H

#endif // ENV5_REGISTERABSTRACTION_H

#include "stdint.h"

uint32_t rightShift(uint32_t input, uint8_t num);
uint32_t leftShift(uint32_t input, uint8_t num);
uint32_t rightRotate(uint32_t input, uint8_t num);
uint32_t leftRotate(uint32_t input, uint8_t num);
uint32_t bitwiseXor(uint32_t inputOne, uint32_t inputTwo);
uint32_t bitwiseNot(uint32_t input);
uint32_t smallSigmaZero(uint32_t input);
uint32_t smallSigmaOne(uint32_t input);
uint32_t bigSigmaOne(uint32_t input);
uint32_t bigSigmaZero(uint32_t input);
uint32_t majority(uint32_t a, uint32_t b, uint32_t c);
uint32_t choice(uint32_t e, uint32_t f, uint32_t g);
uint32_t additionOfWords(uint32_t a, uint32_t b);
uint32_t calculationOfWords(uint32_t wZero, uint32_t wOne, uint32_t wNine, uint32_t wFourteen);