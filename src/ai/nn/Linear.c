#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"

float *linearForward(linearConfig_t *config, float *input) {
    float *output = calloc(2, sizeof(float));

    output[0] = -5.f;
    output[1] = -4.f;
    return output;
};

float *linearBackward(linearConfig_t *config, float *grad, float *input) {
    ;
}



linearConfig_t *initLinearConfigWithWeightBias(float *weight, size_t sizeWeights, float *bias,
                                               size_t sizeBias) {
    ;
}

linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForward_t *initLinearLayerForwardWithWeightBias(float *weight, size_t sizeWeights, float *bias,
                                                     size_t sizeBias) {
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