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
    float *output = input;
    for (size_t i = 0; i < sizeNetwork; i++) {
        output = network[i]->forward(network[i]->config, input);
    }
    return output;
}

trainingStats_t *sequentialCalculateGrads(layerForwardBackward_t **network, size_t sizeNetwork,
                                          void *lossFunction, float *input, float *label) {

    // Array of Pointers
    float **layerOutputs = malloc((sizeNetwork + 1) * sizeof(float *));
    layerOutputs[0] = input;

    // Forward Pass
    for (size_t i = 1; i < sizeNetwork + 1; i++) {
        layerOutputs[i] = network[i-1]->forward(network[i-1]->config, layerOutputs[i-1]);
    }

    // Determine Output Size
    size_t outputSize = 0;
    switch (network[sizeNetwork - 1]->type) {
    case LINEAR:
        linearConfig_t *linearConfig = network[sizeNetwork-1]->config;
        outputSize = linearConfig->outputSize;
        break;
    case RELU:
        ReLUConfig_t *reluConfig = network[sizeNetwork-1]->config;
        outputSize = reluConfig->size;
        break;
    default:
        break;
    }

    // Loss
    float *grad = lossFunction(layerOutputs[sizeNetwork], label, outputSize);
    trainingStats_t  *trainingStats = calloc(1, sizeof(trainingStats_t));
    trainingStats->loss = grad;

    // Backward Pass
    for (size_t i = sizeNetwork; i-- > 0;) {
        grad = network[i]->backward(network[i]->config, grad, layerOutputs[i-1]);
    }

    trainingStats->output = layerOutputs[sizeNetwork];

    return trainingStats;
}
