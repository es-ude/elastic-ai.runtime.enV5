#include "Rounding.h"

#include <stdlib.h>
#include <tgmath.h>

int32_t roundHTE(float input) {
    int32_t output = ceil(input);
    if (output % 2 == 0) {
        return output;
    }
    return output - 1;
}

float randfloat()
{
    return (float)rand()/((float)RAND_MAX+1);
}

int32_t roundSRHTE(const float input) {
    return roundHTE(input+randfloat()-0.5f);
}

int32_t roundByMode(const float input, const roundingMode_t roundingMode) {
    switch (roundingMode) {
    case HTE:
        return roundHTE(input);
    case SRHTE:
        return roundSRHTE(input);
    }
    return 0;
}