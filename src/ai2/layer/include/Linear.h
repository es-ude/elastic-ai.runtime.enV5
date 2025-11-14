#ifndef ENV5_RUNTIME_LINEAR_H
#define ENV5_RUNTIME_LINEAR_H
#include "Tensor.h"
#include "Layer.h"

typedef enum linearType {
    FLOATLAYER,
    ASYMLAYER
}linearQType_t;

typedef struct linearConfig
{
    parameter_t* weights;
    parameter_t* bias;
    linearQType_t qType;
} linearConfig_t;

void initLinearConfig(linearConfig_t* linearConfig, linearQType_t qType, parameter_t* weights, parameter_t* bias);
void linearForward(void *config, tensor_t *input, tensor_t *output);

void linearBackward(void *config, tensor_t* loss, tensor_t* output, tensor_t* propLossTensor);

void calcWeightGradsFloat32(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads);
void calcBiasGradsFloat32(tensor_t *biasGrads, tensor_t *loss);
void calcPropLossFloat32(tensor_t *weights, tensor_t *loss, tensor_t *propLoss);

void calcWeightGradsAsym(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads);
void calcBiasGradsAsym(tensor_t *biasGrads, tensor_t *loss);
void calcPropLossAsym(tensor_t *weights, tensor_t *loss, tensor_t *propLoss);

void initLinearLayer(layer_t *layer, linearConfig_t *linearConfig);
#endif // ENV5_RUNTIME_LINEAR_H
