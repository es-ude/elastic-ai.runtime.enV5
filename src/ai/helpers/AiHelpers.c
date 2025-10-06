#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"
#include "Conv1d.h"
#include "Linear.h"
#include "Softmax.h"
#include "ReLU.h"
#include "CrossEntropy.h"
#include "MSE.h"

#include <string.h>

const layerFunctionEntry_t layerFunctions[] = {
    [LINEAR] = {linearForward, linearBackward},
    [RELU] = {ReLUForward, ReLUBackward},
    [CONV1D] = {conv1dForward, conv1dBackward},
    [SOFTMAX] = {softmaxForward, softmaxBackward}
};

const lossFunctionEntry_t lossFuntions[] = {
    [MSE] = {MSELossBackward},
    [CROSS_ENTROPY] = {crossEntropySoftmaxBackward}
};

parameter_t *initParameter(float *p, size_t size) {
    parameter_t *param = calloc(1, sizeof(parameter_t));

    param->p = calloc(size, sizeof(float));
    memcpy(param->p, p, size * sizeof(float));

    param->size = size;
    param->grad = calloc(size, sizeof(float));
    return param;
}


float *sequentialForward(layerForward_t **network, size_t sizeNetwork, float *input,
                         size_t inputSize) {
    for (size_t i = 0; i < sizeNetwork; i++) {
        layerType_t type = network[i]->type;
        forward *fwd = layerFunctions[type].forwardFunc;
        input = fwd(network[i]->config, input, inputSize);
    }
    return input;
}

size_t calculateInputSizeForNextLayer(layerForwardBackward_t *currentLayer,
                                      size_t inputSizeCurrentLayer) {
    size_t inputSizeNextLayer = -1;
    switch (currentLayer->type) {
    case LINEAR:
        inputSizeNextLayer = ((linearConfig_t *)currentLayer->config)->outputSize;
        break;
    case RELU:
        inputSizeNextLayer = ((ReLUConfig_t *)currentLayer->config)->size;
        break;
    case CONV1D:
        inputSizeNextLayer =
            calcOutputSizeForInputSizeAndConv1dConfig(inputSizeCurrentLayer, currentLayer->config);
        break;
    case SOFTMAX:
        inputSizeNextLayer = ((softmaxConfig_t *)currentLayer->config)->size;
        break;
    default:
        printf("SWITCH CASE NOT FOUND\n");
        break;
    }
    return inputSizeNextLayer;
}

loss *getLossFunctionByType(lossFunctionType_t lossType) {
    loss *lossFunction = malloc(sizeof(loss));
    switch (lossType) {
    case MSE:
        lossFunction = MSELossBackward;
        break;
    case CROSS_ENTROPY:
        lossFunction = crossEntropySoftmaxBackward;
        break;
    default:
        printf("Loss type not found");
        break;
    }
    return lossFunction;
}

trainingStats_t *sequentialCalculateGrads(layerForwardBackward_t **network,
                                          size_t sizeNetwork,
                                          lossFunctionType_t lossFunctionType,
                                          float *input,
                                          size_t inputSize,
                                          float *label) {

    float **layerOutputs = malloc((sizeNetwork + 1) * sizeof(float *));
    layerOutputs[0] = input;

    size_t *layerInputSizes = malloc((sizeNetwork + 1) * sizeof(size_t));
    layerInputSizes[0] = inputSize;

    // Forward Pass
    for (size_t i = 0; i < sizeNetwork; i++) {
        layerForwardBackward_t *currentLayer = network[i];

        forward *fwd = layerFunctions[currentLayer->type].forwardFunc;
        layerOutputs[i + 1] = fwd(currentLayer->config, layerOutputs[i], layerInputSizes[i]);

        layerInputSizes[i + 1] = calculateInputSizeForNextLayer(currentLayer, layerInputSizes[i]);
    }

    layerInputSizes[sizeNetwork] = calculateInputSizeForNextLayer(
        network[sizeNetwork - 1], layerInputSizes[sizeNetwork - 1]);

    // Loss
    loss *lossFunction = getLossFunctionByType(lossFunctionType);
    float *grad = lossFunction(layerOutputs[sizeNetwork], label, layerInputSizes[sizeNetwork]);
    trainingStats_t *trainingStats = calloc(1, sizeof(trainingStats_t));
    trainingStats->loss = grad;


    size_t backwardIndex = sizeNetwork - 1;
    if(lossFunctionType == CROSS_ENTROPY) {
        backwardIndex -= 1;
    }
    // Backward Pass
    for (int i = (int)(backwardIndex); i >= 0; i--) {

        backward *bwd = layerFunctions[network[i]->type].backwardFunc;
        grad = bwd(network[i]->config, grad, layerOutputs[i], layerInputSizes[i]);
    }

    trainingStats->output = grad;

    return trainingStats;
}
