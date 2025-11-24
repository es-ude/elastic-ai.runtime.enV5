#include "SGD.h"
#include "Tensor.h"
#include "Layer.h"
#include "Linear.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct numParams {
    size_t numParams;
    parameter_t *parameters;
} numParams_t;

void initMomentumBuffer(momentumBuffer_t *momentumBuffer, parameter_t *parameter,
                        float *momentums) {
    momentumBuffer->parameter = parameter;
    momentumBuffer->momentums = momentums;
}

uint32_t calcNumberOfMomentumBuffersByLayerType(const layerType_t type) {
    switch (type) {
    case LINEAR:
        return 2;
    case RELU:
        return 0;
    case CONV1D:
        return 2;
    default:
        return 0;
    }
}

void addParameterToMomentumBuffers(momentumBuffer_t *momentumBuffers, parameter_t *parameter,
                                   float *momentums, size_t bufferIndex) {
    momentumBuffers[bufferIndex].parameter = parameter;
    momentumBuffers[bufferIndex].momentums = momentums;
}

uint32_t calcTotalNumberOfMomentumBuffers(layer_t *model, size_t sizeModel) {
    uint32_t number = 0;
    for (size_t i = 0; i < sizeModel; i++) {
        number += calcNumberOfMomentumBuffersByLayerType(model[i].type);
    }
    return number;
}

void initSGDConfig(SGDConfig_t *config, float learningRate, float momentumFactor, float weightDecay,
                   momentumBuffer_t **momentumBuffers, size_t sizeMomentumBuffers) {
    config->learningRate = learningRate;
    config->momentumFactor = momentumFactor;
    config->weightDecay = weightDecay;
    config->momentumBuffers = momentumBuffers;
    config->sizeMomentumBuffers = sizeMomentumBuffers;
}


void SGDStepFloat(SGDConfig_t *config) {

    for (size_t i = 0; i < config->sizeMomentumBuffers; i++) {
        parameter_t *param = config->momentumBuffers[i]->parameter;
        float *momentums = config->momentumBuffers[i]->momentums;

        size_t paramSize = calcNumberOfElementsByParameter(param);
        float *gradFloat = (float *)param->grad->data;
        float *dataFloat = (float *)param->param->data;

        for (size_t j = 0; j < paramSize; ++j) {
            float grad = gradFloat[j] + config->weightDecay * dataFloat[j];
            momentums[j] = config->momentumFactor * momentums[j] + grad;
            dataFloat[j] -= config->learningRate * momentums[j];
        }
    }
}

void SGDZeroGradFloat(SGDConfig_t *config) {
    for (size_t i = 0; i < config->sizeMomentumBuffers; i++) {
        parameter_t *param = config->momentumBuffers[i]->parameter;
        size_t paramSize = calcNumberOfElementsByParameter(param);

        float *gradFloat = (float *)param->grad->data;

        for (size_t j = 0; j < paramSize; ++j) {
            gradFloat[j] = 0.0f;
        }
    }
}
