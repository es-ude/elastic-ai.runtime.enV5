#include "include/DTypes.h"

#include <stdio.h>
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

float findMaxFloat(uint8_t *bytes, size_t numberOfElements, size_t bytesPerElement) {
    float max = readBytesAsFloat(&bytes[0]);
    for (size_t i = 1; i < numberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        float current = readBytesAsFloat(&bytes[byteIndex]);
        if (current > max) {
            max = current;
        }
    }
    return max;
}

float findMinFloat(uint8_t *bytes, size_t numberOfElements, size_t bytesPerElement) {
    float min = readBytesAsFloat(&bytes[0]);
    for (size_t i = 1; i < numberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        float current = readBytesAsFloat(&bytes[byteIndex]);
        if (current < min) {
            min = current;
        }
    }
    return min;
}

quantization_t *initQuantization(dtype_t type) {
    quantization_t *quantization = calloc(1, sizeof(quantization_t));

    switch (type) {
    case FLOAT32:
        quantization->type = FLOAT32;
        return quantization;
    case INT32:
        quantization->type = INT32;
        return quantization;
    case LINEAR:
        quantization->type = LINEAR;
        quantization->qConfig = calloc(1, sizeof(linearQ_t));
        linearQ_t *linearConfig = quantization->qConfig;
        linearConfig->qMax = 256;
        return quantization;
    default:
        return NULL;
    }
}

float convertInt32ToFloat(int32_t value) {
    return (float)value;
}

int32_t convertFloatToInt32(float value) {
    return (int32_t)value;
}
