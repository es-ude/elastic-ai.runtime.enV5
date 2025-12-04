#ifndef SGD_H
#define SGD_H
#include "Tensor.h"
#include "Layer.h"

#include <stddef.h>
#include <stdint.h>

typedef struct momentumBuffer
{
    parameter_t* parameter;
    tensor_t* momentums;
} momentumBuffer_t;

typedef struct SGDConfig
{
    float learningRate;
    float momentumFactor;
    float weightDecay;
    momentumBuffer_t** momentumBuffers;
    size_t sizeMomentumBuffers;
} SGDConfig_t;

void initMomentumBuffer(momentumBuffer_t* momentumBuffer, parameter_t* parameter, tensor_t* momentums);

uint32_t calcTotalNumberOfMomentumBuffers(layer_t* model, size_t sizeModel);

void initSGDConfig(SGDConfig_t* config, float learningRate, float momentumFactor, float weightDecay,
                   momentumBuffer_t** momentumBuffers, size_t sizeMomentumBuffers);

void SGDStepFloat(SGDConfig_t* config);

void SGDStepAsym(SGDConfig_t* config);

void SGDZeroGrad(SGDConfig_t* sgdConfig);

#endif
