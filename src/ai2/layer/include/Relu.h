
#ifndef ENV5_RUNTIME_RELU_H
#define ENV5_RUNTIME_RELU_H
#include "Tensor.h"
#include "Layer.h"


void reluForward(void *config, tensor_t *input, tensor_t *output);
void reluBackward(void *config, tensor_t *input, tensor_t *gradOutputFromPreviousLayer, tensor_t *gradInput);

void initReluLayer(layer_t *layer);


#endif // ENV5_RUNTIME_RELU_H
