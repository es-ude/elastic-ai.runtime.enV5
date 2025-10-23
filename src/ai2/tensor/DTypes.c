#include "DTypes.h"

#include <stdlib.h>
#include <string.h>

int32_t readBytesAsInt32(uint8_t *bytes) {
    int32_t x;
    memcpy(&x, bytes, sizeof(int32_t));
    return x;
}

float readBytesAsFloat(uint8_t *bytes) {
    float x;
    memcpy(&x, bytes, sizeof(float));
    return x;
}
