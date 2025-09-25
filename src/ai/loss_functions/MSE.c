#define SOURCE_FILE "AI_LF_MSE"

#include "MSE.h"

#include <stdio.h>

float *MSELossBackward(float *output, float *label, size_t size) {
    float *result = calloc(size, sizeof(float));
    float mean = 2.f / (float)size;

    for (size_t i = 0; i < size; i++) {
        result[i] = mean * (output[i] - label[i]);
    }
    return result;
}
