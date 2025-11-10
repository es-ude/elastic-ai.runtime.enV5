#ifndef MINMAX_H
#define MINMAX_H

#include <stddef.h>
#include <stdint.h>

float findMaxFloat(uint8_t *bytes, size_t numberOfElements);
float findMinFloat(uint8_t *bytes, size_t numberOfElements);

int32_t findMaxInt32(uint8_t *bytes, size_t numberOfElements);
int32_t findMinInt32(uint8_t *bytes, size_t numberOfElements);

#endif //MINMAX_H
