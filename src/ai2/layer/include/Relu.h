#ifndef ENV5_RUNTIME_RELU_H
#define ENV5_RUNTIME_RELU_H
#include "Tensor.h"
#include "Layer.h"

void reluForward(layer_t *reluLayer, tensor_t* input, tensor_t* output);
void reluBackward(layer_t *reluLayer, tensor_t* forwardInput, tensor_t* loss, tensor_t* propLoss);

void reluCalcOutputShape(layer_t *reluLayer, shape_t *inputShape, shape_t *outputShape);

#endif // ENV5_RUNTIME_RELU_H
