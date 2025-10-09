#include "Softmax.h"

#include <math.h>
#include <string.h>

#define EULER_APPROX = 2.71828

softmaxConfig_t *initSoftmaxConfig() {
    softmaxConfig_t *config = calloc(1, sizeof(softmaxConfig_t));
    return config;
}

layerForward_t *initSoftmaxLayerForward() {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = SOFTMAX;
    layerForward->config = initSoftmaxConfig(0);
    return layerForward;
}

layerForwardBackward_t *initSoftmaxLayerForwardBackward() {
    layerForwardBackward_t *layerBackward = calloc(1, sizeof(layerForward_t));
    layerBackward->type = SOFTMAX;
    layerBackward->config = initSoftmaxConfig(0);
    return layerBackward;
}


tensor_t *softmaxForward(void *config, tensor_t *inputTensor) {
    size_t totalInputSize = calcTotalNumberOfElementsByTensor(inputTensor);

    tensor_t *outputTensor = calloc(1, sizeof(tensor_t));
    outputTensor->data = calloc(totalInputSize, sizeof(float));
    outputTensor->numberOfDimensions = inputTensor->numberOfDimensions;
    outputTensor->dimensions = calloc(outputTensor->numberOfDimensions, sizeof(size_t));
    memcpy(outputTensor->dimensions, inputTensor->dimensions, outputTensor->numberOfDimensions * sizeof(size_t));

    float sum = 0;
    for (size_t i = 0; i < totalInputSize; i++) {
        sum += expf(inputTensor->data[i]);
    }

    for (size_t i = 0; i < totalInputSize; i++) {
        outputTensor->data[i] = expf(inputTensor->data[i]) / sum;
    }

    return outputTensor;
}

float *softmaxBackward(void *config, float *grad, float *input, size_t inputSize) {
    softmaxConfig_t *softmaxConfig = config;

    float *output = calloc(inputSize, sizeof(float));

    float jacobian[inputSize][inputSize];

    for (size_t i = 0; i < inputSize; i++) {
        for (size_t j = 0; j < inputSize; j++) {
            if (i == j) {
                jacobian[i][j] = input[i] * (1 - input[i]);
            } else {
                jacobian[i][j] = -input[i] * input[j];
            }
        }
    }

    for (size_t i = 0; i < inputSize; i++) {
        float sum = 0;
        for (size_t j = 0; j < inputSize; j++) {
            sum += jacobian[i][j] * grad[j];
        }
        output[i] = sum;
    }

    return output;
}


void freeSoftmaxLayerForward(layerForward_t *layer) {}
void freeSoftmaxLayerBackward(layerForwardBackward_t *layer) {}
