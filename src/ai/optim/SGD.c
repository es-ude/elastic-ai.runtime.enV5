#define SOURCE_FILE "AI_OPTIM_SGD"

#include "SGD.h"

#include "../nn/include/Conv1d.h"
#include "../nn/include/Linear.h"
#include "Common.h"
#include <stdio.h>
#include <inttypes.h>

typedef struct numParams{
    size_t numParams;
    parameter_t **parameters;
}numParams_t;

momentumBuffer_t * initMomentumBuffer(parameter_t *parameter) {
    momentumBuffer_t *momentumBuffer = calloc(1, sizeof(momentumBuffer_t));
    momentumBuffer->parameter = parameter;
    momentumBuffer->momentums = calloc(parameter->size, sizeof(float));
    return momentumBuffer;
}


void initMomentumBufferWithBuffer(parameter_t *parameter, momentumBuffer_t *momentum_buffer) {
    momentum_buffer->parameter = parameter;
}

uint32_t getNumberOfMomentumBuffersByLayerType(layerType_t type) {
    switch (type) {
    case LINEAR: return 2; break;
    case RELU: return 0; break;
    case CONV1D: return 2; break;
    default: return 0; break;
    }
}

void getParamPointerInMomentumBuffersByLayerType(layerForwardBackward_t *layerBackward, momentumBuffer_t **momentum_buffer, uint32_t index) {
    switch (layerBackward->type) {
    case LINEAR:
        const linearConfig_t *linearConfig = layerBackward->config;
        momentum_buffer[index]->parameter = linearConfig->weight;
        momentum_buffer[index]->momentums = calloc(linearConfig->weight->size, sizeof(float));
        momentum_buffer[index+1]->parameter = linearConfig->bias;
        momentum_buffer[index+1]->momentums = calloc(linearConfig->bias->size, sizeof(float));
        break;
    case RELU:
        break;
    case CONV1D:
        const conv1dConfig_t *conv1dConfig = layerBackward->config;
        momentum_buffer[index]->parameter = conv1dConfig->weight;
        momentum_buffer[index]->momentums = calloc(conv1dConfig->weight->size, sizeof(float));
        momentum_buffer[index+1]->parameter = conv1dConfig->bias;
        momentum_buffer[index+1]->momentums = calloc(conv1dConfig->bias->size, sizeof(float));
        break;
    }
}

uint32_t getNumberOfMomentumBuffer(layerForwardBackward_t **layerBackward, size_t sizeModel) {
    uint32_t number = 0;
    for (int i = 0; i < sizeModel; i++) {
        number += getNumberOfMomentumBuffersByLayerType(layerBackward[i]->type);
    }
    return number;
}

SGDConfig_t *initSGDConfig(layerForwardBackward_t **model, size_t sizeModel, float lr, float momentum, float weightDecay) {
    SGDConfig_t *config = calloc(1, sizeof(SGDConfig_t));
    config->lr = lr;
    config->momentum = momentum;
    config->weightDecay = weightDecay;
    uint32_t numberMomentumBuffer = getNumberOfMomentumBuffer(model, sizeModel);
    config->sizeMomentumBuffers = numberMomentumBuffer;
    config->momentum_buffer = calloc(numberMomentumBuffer, sizeof(uintptr_t));

    // init each item in the array of config->momentum_buffer with a momentumbuffer
    for (int i = 0; i < numberMomentumBuffer; i++) {
        config->momentum_buffer[i] = calloc(1, sizeof(momentumBuffer_t));
    }


    // fill each item of the config->momentum_buffer with the corresponding parameters
    numberMomentumBuffer = 0;
    for (uint32_t i = 0; i < sizeModel; i++) {
        getParamPointerInMomentumBuffersByLayerType(model[i], config->momentum_buffer, numberMomentumBuffer);
        numberMomentumBuffer += getNumberOfMomentumBuffersByLayerType(model[i]->type);
    }
    return config;
}


void SGDStep(const SGDConfig_t *config) {
    for (size_t i = 0; i < config->sizeMomentumBuffers; ++i) {
        const parameter_t *param = config->momentum_buffer[i]->parameter;
        float *momentum = config->momentum_buffer[i]->momentums;

        for (size_t j = 0; j < param->size; ++j) {
            const float grad = param->grad[j] + config->weightDecay * param->p[j];
            momentum[j] = config->momentum * momentum[j] + grad;
            param->p[j] -= config->lr * momentum[j];
        }
    }
}

void SGDZeroGrad(const SGDConfig_t *config) {
    for (size_t i = 0; i < config->sizeMomentumBuffers; ++i) {
        const parameter_t *param = config->momentum_buffer[i]->parameter;
        for (size_t j = 0; j < param->size; ++j) {
            param->grad[j] = 0.0f;
        }
    }
}
