#ifndef ENV5_RUNTIME_LINEAR_H
#define ENV5_RUNTIME_LINEAR_H
#include "Tensor.h"

typedef enum linearType {
    FLOATLAYER,
    ASYMLAYER
}linearType_t;

typedef struct linearConfig
{
    parameter_t* weight;
    parameter_t* bias;
    linearType_t type;
} linearConfig_t;

void linearForward(void *config, tensor_t *input, tensor_t *output);

void linearBackward(void *config, tensor_t* loss, tensor_t* output, tensor_t* propLossTensor);

void calcWeightGradsFloat32(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads);
void calcBiasGradsFloat32(tensor_t *biasGrads, tensor_t *loss);
void calcPropLossFloat32(tensor_t *weights, tensor_t *loss, tensor_t *propLoss);

void calcWeightGradsAsym(tensor_t *loss, tensor_t *forwardInput, tensor_t *weightGrads);
void calcBiasGradsAsym(tensor_t *biasGrads, tensor_t *loss);
void calcPropLossAsym(tensor_t *weights, tensor_t *loss, tensor_t *propLoss);
#endif // ENV5_RUNTIME_LINEAR_H
