#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"

float *linearForward(linearConfig_t *config, float *input) {
    size_t inputSize = config->inputSize;
    size_t outputSize = config->outputSize;
    size_t weightIndex = 0;

    float *output = calloc(outputSize, sizeof(float));

    for (size_t i = 0; i < outputSize; i++) {
        float result = 0;
        for (size_t j = 0; j < inputSize; j++) {
            weightIndex = i * inputSize + j;
            result += input[j] * config->weight.p[weightIndex];
        }
        output[i] = result + config->bias.p[i];
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

            config->weight.grad[weightIndex] = grad[lossIndex] * input[inputIndex];
            propagatedLoss[inputIndex] += config->weight.p[inputIndex + lossIndex * inputSize] * grad[lossIndex];
        }
        config->bias.grad[lossIndex] = grad[lossIndex];
    }

    return propagatedLoss;
}

    linearConfig_t *initLinearConfigWithWeightBias(float *weight, size_t sizeWeights, float *bias,
                                                   size_t sizeBias) {
        ;
    }

    linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize) {
        ;
    }

    layerForward_t *initLinearLayerForwardWithWeightBias(float *weight, size_t sizeWeights,
                                                         float *bias, size_t sizeBias) {
        ;
    }

    layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize) {
        ;
    }

    layerForwardBackward_t *initLinearLayerBackwardWithWeightBias(float *weight, size_t sizeWeights,
                                                                  float *bias, size_t sizeBias) {
        ;
    }

    layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize,
                                                                       size_t outputSize) {
        ;
    }
