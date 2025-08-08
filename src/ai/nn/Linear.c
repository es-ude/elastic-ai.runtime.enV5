#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"

#include <Common.h>

float *linearForward(linearConfig_t *config, float *input) {
    size_t inputSize = config->inputSize;
    size_t outputSize = config->outputSize;
    size_t weightIndex = 0;

    float *output = malloc(outputSize * sizeof(float));

    for (size_t i = 0; i < outputSize; i++) {
        float result = 0;
        for (size_t j = 0; j < inputSize; j++) {
            weightIndex = i * inputSize + j;
            result += input[j] * config->weight->p[weightIndex];
        }
        output[i] = result + config->bias->p[i];
    }

    return output;
}

float *linearBackward(linearConfig_t *config, float *grad, float *input) {
    size_t inputSize = config->inputSize;
    size_t outputSize = config->outputSize;

    float *propagatedLoss = calloc(inputSize, sizeof(float));

    size_t weightIndex = 0;
    for (size_t lossIndex = 0; lossIndex < outputSize; lossIndex++) {
        for (size_t inputIndex = 0; inputIndex < inputSize; inputIndex++) {
            weightIndex = lossIndex * inputSize + inputIndex;

            config->weight->grad[weightIndex] = grad[lossIndex] * input[inputIndex];
            propagatedLoss[inputIndex] += config->weight->p[weightIndex] * grad[lossIndex];
        }
        config->bias->grad[lossIndex] = grad[lossIndex];
    }

    return propagatedLoss;
}

linearConfig_t *initLinearConfigWithWeightBias(float *weight, size_t sizeWeights, float *bias,
                                               size_t sizeBias) {

    linearConfig_t *config = calloc(1, sizeof(linearConfig_t));
    config->weight = initParameter(weight, sizeWeights);
    config->bias = initParameter(bias, sizeBias);
    config->inputSize = sizeWeights / sizeBias;
    config->outputSize = sizeBias;

    return config;
}

linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForward_t *initLinearLayerForwardWithWeightBias(float *weight, size_t sizeWeights, float *bias,
                                                     size_t sizeBias) {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->config = initLinearConfigWithWeightBias(weight, sizeWeights, bias, sizeBias);
    layerForward->type = LINEAR;
    layerForward->layerForward = &linearForward;
    return layerForward;
}

layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForwardBackward_t *initLinearLayerForwardBackwardWithWeightBias(float *weight,
                                                                     size_t sizeWeights,
                                                                     float *bias, size_t sizeBias) {
    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForwardBackward_t));
    layerForwardBackward->config =
        initLinearConfigWithWeightBias(weight, sizeWeights, bias, sizeBias);
    layerForwardBackward->type = LINEAR;
    layerForwardBackward->layerForward = &linearForward;
    layerForwardBackward->layerBackward = &linearBackward;
    return layerForwardBackward;
}

layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize,
                                                                   size_t outputSize) {
    ;
}
