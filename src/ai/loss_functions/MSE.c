#define SOURCE_FILE "AI_LF_MSE"

#include "MSE.h"

#include <stdio.h>

tensor_t *MSELossBackward(tensor_t *output,tensor_t *label) {
    size_t totalSize = calcTotalNumberOfElementsByTensor(output);

    tensor_t *result = malloc(sizeof(tensor_t));
    result->data = malloc(totalSize * sizeof(float));
    result->dimensions = malloc(output->numberOfDimensions * sizeof(size_t));

    result->numberOfDimensions = output->numberOfDimensions;
    result->dimensions = output->dimensions;

    float mean = 2.f / (float)totalSize;

    for (size_t i = 0; i < totalSize; i++) {
        result->data[i] = mean * (output->data[i] - label->data[i]);
    }
    return result;
}
