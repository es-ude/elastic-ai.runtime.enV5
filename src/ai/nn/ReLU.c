#define SOURCE_FILE "AI_NN_RELU"

#include "ReLU.h"

ReLUConfig_t *initReLUConfig(size_t size) {
    ReLUConfig_t *config = calloc(1, sizeof(ReLUConfig_t));
    config->size = size;
    return config;
}

layerForward_t *initReLULayerForward(size_t size) {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = RELU;
    layerForward->config = initReLUConfig(size);
    return layerForward;
}

layerForwardBackward_t *initReLULayerForwardBackward(size_t size) {
    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForward_t));
    layerForwardBackward->type = RELU;
    layerForwardBackward->config = initReLUConfig(size);
    return layerForwardBackward;
}

void freeReLUForward(layerForward_t *layer) {}

void freeReLUBackward(layerForwardBackward_t *layer) {}


float *ReLUForward(void *config, float *input) {
    ReLUConfig_t *reluConfig = config;

    float *output = calloc(reluConfig->size, sizeof(float));
    for (size_t index = 0; index < reluConfig->size; index++) {
        if (input[index] < 0.0f) {
            output[index] = 0.f;
        } else {
            output[index] = input[index];
        }
    }
    return output;
}

float *ReLUBackward(void *config, float *grad, float *input) {
    ReLUConfig_t *reluConfig = config;

    float *outputGrad = calloc(reluConfig->size, sizeof(float));

    for (size_t i = 0; i < reluConfig->size; i++) {
        if (input[i] <= 0.0f) {
            outputGrad[i] = 0.0f;
        } else {
            outputGrad[i] = grad[i];
        }
    }
    return outputGrad;
}
