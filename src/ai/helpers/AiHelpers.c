#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"
#include "Linear.h"
#include "ReLU.h"

#include <string.h>

parameter_t *initParameter(float *p, size_t size) {
    parameter_t *param = calloc(1, sizeof(parameter_t));

    param->p = calloc(size, sizeof(float));
    memcpy(param->p, p, size * sizeof(float));

    param->size = size;
    param->grad = calloc(size, sizeof(float));
    return param;
}



float *sequentialForward(layerForward_t **network, size_t sizeNetwork, float *input) {
    float *output = 0;
    for (size_t i = 0; i < sizeNetwork; i++) {
        network[i]->layerForward(network[i]->config, input);
        switch (network[i]->type) {
        case LINEAR:
            output = linearForward(network[i]->config, input);
            break;
        case RELU:
            output = ReLUForward(network[i]->config, input);
            break;
        case CONV1D:
            break;
        }
    }

    return output;
}

float *sequentialCalculateGrads(layerForwardBackward_t **network, size_t sizeNetwork,
                                void *lossFunction, float *input, float *label) {

    // Array of Pointers
    float **layerOutputs = malloc((sizeNetwork + 1) * sizeof(float *));
    layerOutputs[0] = input;

    // Forward Pass
    for (size_t i = 0; i < sizeNetwork; i++) {
        layerOutputs[i + 1] = network[i]->layerForward(network[i].config, layerOutputs[i]);
    }

    // Determine Output Size
    size_t outputSize = 0;
    switch (network[sizeNetwork - 1].type) {
    case LINEAR:
        linearConfig_t *linearConfig = network[sizeNetwork].config;
        outputSize = linearConfig->outputSize;
        break;
    case RELU:
        ReLUConfig_t *reluConfig = network[sizeNetwork].config;
        outputSize = reluConfig->size;
        break;
    case CONV1D:
        break;
    }

    // Loss
    float *grad = lossFunction(layerOutputs[sizeNetwork], label, outputSize);

    // Backward Pass
    for (size_t i = sizeNetwork; i-- > 0;) {
        grad = network[i].layerBackward(network[i].config, grad, layerOutputs[i]);
    }

    float *output = layerOutputs[sizeNetwork];
    free(layerOutputs);

    return output;
}
