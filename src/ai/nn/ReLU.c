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
    layerForward->layerForward = ReLUForwardAutomatic;
    return layerForward;
}

layerForwardBackward_t *initReLULayerBackward(size_t size) {
    layerForwardBackward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = RELU;
    layerForward->config = initReLUConfig(size);
    layerForward->layerForward = ReLUForwardAutomatic;
    layerForward->layerBackward = ReLUBackwardAutomatic;
    return layerForward;
}

void freeReLUForward(layerForward_t *layer ) {

}

void freeReLUBackward(layerForwardBackward_t *layer) {

}


float *ReLUForward(ReLUConfig_t *config, float *input) {

    float *output = calloc(config->size, sizeof(float));
    for (size_t index = 0; index < config->size; index++) {
        if (input[index] < 0.0f) {
            output[index] = 0.f;
        } else {
            output[index] = input[index];
        }
    }
    return output;
}
float *ReLUBackward(ReLUConfig_t *config, float *grad, float *input) {
    float *outputGrad = calloc(config->size, sizeof(float));

    for (size_t i = 0; i < config->size; i++) {
        if (input[i] <= 0.0f) {
            outputGrad[i] = 0.0f;
        } else {
            outputGrad[i] = grad[i];
        }
    }
    return outputGrad;
}

float *ReLUForwardAutomatic(void *config, float *input) {
    ReLUConfig_t *configInternal = config;
    return ReLUForward(configInternal, input);
}

float *ReLUBackwardAutomatic(void *config, float *grad, float *input) {
    ReLUConfig_t *configInternal = config;
    return ReLUBackward(configInternal, grad, input);
}
