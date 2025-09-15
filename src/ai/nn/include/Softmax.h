#ifndef SOFTMAX_H
#define SOFTMAX_H
#include "AiHelpers.h"

#include <stdio.h>

typedef struct softmaxConfig {
    size_t size;
}softmaxConfig_t;

softmaxConfig_t *initSoftmaxConfig(size_t size);

layerForward_t *initSoftmaxLayerForward(size_t size);

layerForwardBackward_t *initSoftmaxLayerBackward(size_t size);

void freeSoftmaxLayerForward(layerForward_t *layer);
void freeSoftmaxLayerBackward(layerForwardBackward_t *layer);

float *softmaxForward(softmaxConfig_t *config, float *input);

float *softmaxBackward(softmaxConfig_t *config, float *grad, float *input);

#endif // SOFTMAX_H
