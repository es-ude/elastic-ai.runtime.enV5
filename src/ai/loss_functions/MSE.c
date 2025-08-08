#define SOURCE_FILE "AI_LF_MSE"

#include "MSE.h"

float *MSELossDOutput(float *output, float *label, size_t size) {
    float *result = calloc(size, sizeof(float));
    for (size_t i = 0; i < size; i++) {
        result[i] = output[i] - label[i];
    }
    return result;
}
