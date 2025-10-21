#include "Rounding.h"
#include <tgmath.h>

int32_t roundHTE(float input) {
    int32_t output = ceil(input);
    if (output % 2 == 0) {
        return output;
    }
    return output - 1;
}

int32_t roundByMode(float input, roundingMode_t roundingMode) {
    switch (roundingMode) {
    case HTE:
        return roundHTE(input);
    case SRHTE:
        return 0;
    }
    return 0;
}
