#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"
#include "Conv1d.h"
#include "Linear.h"
#include "Softmax.h"
#include "ReLU.h"

#include <string.h>

const layerFunctionEntry_t layerFunctions[] = {
    [LINEAR] = {linearForward, linearBackward},
    [RELU] = {ReLUForward, ReLUBackward},
    [CONV1D] = {conv1dForward, conv1dBackward},
    [SOFTMAX] = {softmaxForward, softmaxBackward}
};

parameter_t *initParameter(float *p, size_t size) {
    parameter_t *param = calloc(1, sizeof(parameter_t));

    param->p = calloc(size, sizeof(float));
    memcpy(param->p, p, size * sizeof(float));

    param->size = size;
    param->grad = calloc(size, sizeof(float));
    return param;
}


float *sequentialForward(layerForward_t **network, size_t sizeNetwork, float *input, size_t inputSize) {
    for (size_t i = 0; i < sizeNetwork; i++) {
        layerType_t type = network[i]->type;
        forward *fwd = layerFunctions[type].forwardFunc;
        input = fwd(network[i]->config, input, inputSize);
    }
    return input;
}


trainingStats_t *sequentialCalculateGrads(layerForwardBackward_t **network,
                                          size_t sizeNetwork,
                                          float *(*lossFunction)(
                                              float *prediction, float *label, size_t outputSize),
                                          float *input,
                                          size_t inputSize,
                                          float *label) {

    float **layerOutputs = malloc((sizeNetwork + 1) * sizeof(float *));
    layerOutputs[0] = input;

    // Forward Pass
    for (size_t i = 0; i < sizeNetwork; i++) {
        forward *fwd = layerFunctions[network[i]->type].forwardFunc;
        layerOutputs[i+1] = fwd(network[i]->config, layerOutputs[i], inputSize);
    }

    // Determine Output Size
    size_t outputSize = 0;
    switch (network[sizeNetwork - 1]->type) {
    case LINEAR:
        linearConfig_t *linearConfig = network[sizeNetwork - 1]->config;
        outputSize = linearConfig->outputSize;
        break;
    case RELU:
        ReLUConfig_t *reluConfig = network[sizeNetwork - 1]->config;
        outputSize = reluConfig->size;
        break;
    case CONV1D:
        conv1dConfig_t *conv1dConfig = network[sizeNetwork-1]->config;
        outputSize = calcOutputSizeForInputSizeAndConv1dConfig(inputSize, conv1dConfig);
        break;
    default:
        printf("SWITCH CASE NOT FOUND\n");
        break;
    }

    // Loss
    float *grad = lossFunction(layerOutputs[sizeNetwork], label, outputSize);
    trainingStats_t *trainingStats = calloc(1, sizeof(trainingStats_t));
    trainingStats->loss = grad;

    // Backward Pass
    for (int i = (int)(sizeNetwork - 1); i >= 0; i--) {
        backward *bwd = layerFunctions[network[i]->type].backwardFunc;
        grad = bwd(network[i]->config, grad, layerOutputs[i], inputSize);
    }

    trainingStats->output = grad;

    return trainingStats;
}

