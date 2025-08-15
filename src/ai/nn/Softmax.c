#include "Softmax.h"


softmaxConfig_t *initSoftmaxConfig(size_t size) {
    softmaxConfig_t *config = calloc(1, sizeof(softmaxConfig_t));
    config->size = size;
    return config;
}

layerForward_t *initSoftmaxLayerForward(size_t size) {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = RELU;
    layerForward->config = initSoftmaxConfig(size);
    return layerForward;
}

layerForwardBackward_t *initSoftmaxLayerBackward(size_t size) {
    layerForwardBackward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = RELU;
    layerForward->config = initSoftmaxConfig(size);
    return layerForward;
}

void freeSoftmaxLayerForward(layerForward_t *layer) {

}
void freeSoftmaxLayerBackward(layerForwardBackward_t *layer) {

}

float *softmaxForward(softmaxConfig_t *config, float *input) {

}

float *softmaxBackward(softmaxConfig_t *config, float *grad, float *input) {

}

float *softmaxForwardAutomatic(void *config, float *input) {

}

float *softmaxBackwardAutomatic(void *config, float *grad, float *input) {

}