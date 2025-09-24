#ifndef SOFTMAX_H
#define SOFTMAX_H

#include "AiHelpers.h"

typedef struct softmaxConfig {
    size_t size;
}softmaxConfig_t;

softmaxConfig_t *initSoftmaxConfig(size_t size);

layerForward_t *initSoftmaxLayerForward(size_t size);

layerForwardBackward_t *initSoftmaxLayerBackward(size_t size);

void freeSoftmaxLayerForward(layerForward_t *layer);
void freeSoftmaxLayerBackward(layerForwardBackward_t *layer);

float *softmaxForward(void *config, float *input, size_t inputSize);

float *softmaxBackward(void *config, float *grad, float *input, size_t inputSize);

#endif // SOFTMAX_H
