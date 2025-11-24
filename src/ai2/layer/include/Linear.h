#ifndef ENV5_RUNTIME_LINEAR_H
#define ENV5_RUNTIME_LINEAR_H
#include "Tensor.h"

struct layer;
typedef struct layer layer_t;


typedef struct linearConfig
{
    parameter_t* weights;
    parameter_t* bias;
} linearConfig_t;

void linearInitConfig(linearConfig_t* linearConfig, parameter_t* weights, parameter_t* bias);

void linearForward(layer_t *linearLayer, tensor_t* input, tensor_t* output);

void linearBackward(layer_t *linearLayer, tensor_t* forwardInput, tensor_t* loss, tensor_t* propLossTensor);

void linearCalcWeightGradsFloat32(tensor_t* loss, tensor_t* forwardInput, tensor_t* weightGrads);
void linearCalcBiasGradsFloat32(tensor_t* biasGrads, tensor_t* loss);
void linearCalcPropLossFloat32(tensor_t* weights, tensor_t* loss, tensor_t* propLoss);

void linearCalcWeightGradsAsym(tensor_t* loss, tensor_t* forwardInput, tensor_t* weightGrads);
void linearCalcBiasGradsAsym(tensor_t* biasGrads, tensor_t* loss);
void linearCalcPropLossAsym(tensor_t* weights, tensor_t* loss, tensor_t* propLoss);

void linearCalcOutputShape(layer_t *linearLayer, shape_t *inputShape, shape_t *outputShape);

#endif // ENV5_RUNTIME_LINEAR_H
