#define SOURCE_FILE "AI_LF_MSE"

#include "MSE.h"

#include <stdio.h>

float *MSELossBackward(float *output, float *label, size_t totalSize) {
    float *result = calloc(totalSize, sizeof(float));
    float mean = 2.f / (float)totalSize;

    for (size_t i = 0; i < totalSize; i++) {
        result[i] = mean * (output[i] - label[i]);
    }
    return result;
}
