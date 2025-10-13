#ifndef SOFTMAX_H
#define SOFTMAX_H

#include "AiHelpers.h"

typedef struct softmaxConfig {
    size_t size;
}softmaxConfig_t;

softmaxConfig_t *initSoftmaxConfig();

layerForward_t *initSoftmaxLayerForward();

layerForwardBackward_t *initSoftmaxLayerForwardBackward();

void freeSoftmaxLayerForward(layerForward_t *layer);
void freeSoftmaxLayerBackward(layerForwardBackward_t *layer);

qTensor_t *softmaxForward(void *config, qTensor_t *inputTensor);

float *softmaxBackward(void *config, float *grad, float *input, size_t inputSize);

#endif // SOFTMAX_H
