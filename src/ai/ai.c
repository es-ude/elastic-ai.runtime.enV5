#define SOURCE_FILE "AI"

#include "ai.h"
#include "math.h"

parameter_t *initParameter(float *p, size_t size) {
    parameter_t* param = calloc(1, sizeof(parameter_t));
    param->p = p;
    param->grad[size];
    param->size = size;
    for (int i = 0; i < size; i++) {
        param->grad[i] = 0;
    }
}

float *sequentialForward(layerForward_t network[], float *input) {
    ;
}
void sequentialCalculateGrads(layerForwardBackward_t network[], void* lossFunction, float *input) {
    ;
}

linearConfig_t *initLinearConfigWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias) {
    ;
}

linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForward_t *initLinearLayerForwardWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias) {
    ;
}

layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForwardBackward_t *initLinearLayerBackwardWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias) {
    ;
}

layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}




float *linearForward(linearConfig_t *config, float *input) {
    float* output = calloc(2, sizeof(float));

    output[0] = -5.f;
    output[1] = -4.f;
    return output;
};

float *linearBackward(linearConfig_t *config, float *grad, float *input) {
    ;
}

ReLUConfig_t *initReLUConfig(size_t size) {
    ;
}


float* ReLUForward(ReLUConfig_t *config, float *input) {
    ;
}
float* ReLUBackward(ReLUConfig_t *config, float *grad, float *input) {
    ;
}

float* MSELossDOutput(float* output, float* label, size_t size) {
    ;
}

momentumBuffer_t *initMomentumBuffer(void *parameter) {

}

SGDConfig_t *initSGDConfig(layerForwardBackward_t *model, size_t sizeModel, float lr, float momentum, float weightDecay) {

}

void SGDStep(SGDConfig_t *SGDConfig) {
    ;
}

void SGDZeroGrad(SGDConfig_t *SGDConfig) {
    ;
}
