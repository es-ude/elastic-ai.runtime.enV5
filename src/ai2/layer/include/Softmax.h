#ifndef ENV5_RUNTIME_SOFTMAX_H
#define ENV5_RUNTIME_SOFTMAX_H
#include "Layer.h"
#include <stddef.h>

typedef struct softmaxConfig
{
    size_t size;
} softmaxConfig_t;

void initSoftmaxLayer(layer_t* softmaxLayer);

void softmaxForward(layer_t* softmaxLayer, tensor_t* input, tensor_t* output);

void softmaxBackward(layer_t* softmaxLayer, tensor_t* input, tensor_t* loss, tensor_t* propLoss);

#endif // ENV5_RUNTIME_SOFTMAX_H
