#define SOURCE_FILE "AI_LF_MSE"

#include "MSE.h"

#include <stdio.h>
#include <string.h>

tensor_t *MSELossBackward(tensor_t *output, tensor_t *label) {
    size_t totalSize = calcTotalNumberOfElementsByTensor(output);

    tensor_t *result = calloc(1, sizeof(tensor_t));
    result->data = calloc(totalSize, sizeof(float));
    result->dimensions = calloc(output->numberOfDimensions,  sizeof(size_t));

    result->numberOfDimensions = output->numberOfDimensions;
    memcpy(result->dimensions, output->dimensions, result->numberOfDimensions);

    float mean = 2.f / (float)totalSize;

    for (size_t i = 0; i < totalSize; i++) {
        result->data[i] = mean * (output->data[i] - label->data[i]);
    }
    return result;
}
