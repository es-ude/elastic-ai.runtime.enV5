#define SOURCE_FILE "AI_NN_RELU"

#include "ReLU.h"

ReLUConfig_t *initReLUConfig(size_t size) {
    ReLUConfig_t *config = calloc(1, sizeof(ReLUConfig_t));
    config->size = size;
    return config;
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


