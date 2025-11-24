#include "Layer.h"
#include "Linear.h"
#include "Relu.h"

layerFunctions_t layerFunctions[] = {
    [LINEAR] = {linearForward, linearBackward, linearCalcOutputShape},
    [RELU] = {reluForward, reluBackward, reluCalcOutputShape},
    [CONV1D] = {NULL, NULL, NULL},
    [SOFTMAX] = {NULL, NULL, NULL}
};

void initLayer(layer_t *layer, layerType_t layerType, layerConfig_t* config, layerQType_t layerQType, quantization_t* inputQ, quantization_t* outputQ) {
    layer->type = layerType;
    layer->config = config;
    layer->qType = layerQType;
    layer->inputQ = inputQ;
    layer->outputQ = outputQ;
}