#ifndef ROUNDING_H
#define ROUNDING_H
#include <stdint.h>

/*! @brief Describes rounding
 * HTE = Half to Even
 * SRHTE = Stochastic Rounding Half to Even
 */
typedef enum roundingMode {
    HTE,
    SRHTE
} roundingMode_t;

int32_t roundByMode(float input, roundingMode_t roundingMode);

float clamp(float input, float min, float max);

#endif //ROUNDING_H
