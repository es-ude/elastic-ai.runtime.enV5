#define SOURCE_FILE "AI_NN_RELU"

#include "ReLU.h"

#include <stdio.h>
#include <string.h>

ReLUConfig_t *initReLUConfig(size_t size) {
    ReLUConfig_t *config = calloc(1, sizeof(ReLUConfig_t));
    config->size = size;
    return config;
}

layerForward_t *initReLULayerForward() {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = RELU;
    layerForward->config = initReLUConfig(0);
    return layerForward;
}

layerForwardBackward_t *initReLULayerForwardBackward() {
    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForwardBackward_t));
    layerForwardBackward->type = RELU;
    layerForwardBackward->config = initReLUConfig(0);
    return layerForwardBackward;
}

void freeReLUForward(layerForward_t *layer) {}

void freeReLUBackward(layerForwardBackward_t *layer) {}


tensor_t *ReLUForward(void *config, tensor_t *inputTensor) {
    size_t numberOfDims = inputTensor->numberOfDimensions;
    size_t *dims = calloc(numberOfDims, sizeof(size_t));
    memcpy(dims, inputTensor->dimensions, numberOfDims * sizeof(size_t)); // WICHTIG: * sizeof(size_t)

    size_t totalOutputSize = calcTotalNumberOfElementsByTensor(inputTensor);

    float *data = calloc(totalOutputSize, sizeof(float));

    for (size_t index = 0; index < totalOutputSize; index++) {
        if (inputTensor->data[index] < 0.0f) {
            data[index] = 0.f;
        } else {
            data[index] = inputTensor->data[index];
        }
    }

    tensor_t *outputTensor = initTensor(data, numberOfDims, dims);

    return outputTensor;
}

tensor_t *ReLUBackward(void *config, tensor_t *gradTensor, tensor_t *inputTensor) {
    tensor_t *outputTensor = calloc(1, sizeof(tensor_t));
    outputTensor->numberOfDimensions = inputTensor->numberOfDimensions;
    outputTensor->dimensions = calloc(outputTensor->numberOfDimensions, sizeof(size_t));
    memcpy(outputTensor->dimensions, inputTensor->dimensions, outputTensor->numberOfDimensions * sizeof(size_t));

    size_t size = calcTotalNumberOfElementsByTensor(inputTensor);
    outputTensor->data = calloc(size, sizeof(float));

    for (size_t i = 0; i < size; i++) {
        if (inputTensor->data[i] <= 0.0f) {
            outputTensor->data[i] = 0.0f;
        } else {
            outputTensor->data[i] = gradTensor->data[i];
        }
    }
    return outputTensor;
}
