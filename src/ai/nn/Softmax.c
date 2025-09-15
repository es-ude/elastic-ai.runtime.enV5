#include "Softmax.h"

#include <math.h>

#define EULER_APPROX = 2.71828

softmaxConfig_t *initSoftmaxConfig(size_t size) {
    softmaxConfig_t *config = calloc(1, sizeof(softmaxConfig_t));
    config->size = size;
    return config;
}

layerForward_t *initSoftmaxLayerForward(size_t size) {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = SOFTMAX;
    layerForward->config = initSoftmaxConfig(size);
    return layerForward;
}

layerForwardBackward_t *initSoftmaxLayerBackward(size_t size) {
    layerForwardBackward_t *layerBackward = calloc(1, sizeof(layerForward_t));
    layerBackward->type = SOFTMAX;
    layerBackward->config = initSoftmaxConfig(size);
    return layerBackward;
}


float *softmaxForward(softmaxConfig_t *config, float *input, size_t inputSize) {
    float *output = calloc(inputSize, sizeof(float));

    float sum = 0;
    for (size_t i = 0; i < inputSize; i++) {
        sum += expf(input[i]);
    }

    for (size_t i = 0; i < inputSize; i++) {
        output[i] = expf(input[i]) / sum;
    }

    return output;
}

float *softmaxBackward(softmaxConfig_t *config, float *grad, float *input, size_t inputSize) {
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
