#ifndef MODEL_API_H
#define MODEL_API_H
#include "Layer.h"
#include "Tensor.h"

typedef struct trainingStats {
    tensor_t *output;
    tensor_t *loss;
} trainingStats_t;

void inference(layer_t **model, size_t numberOfLayers, tensor_t *input, tensor_t *output);

void calculateGrads(layer_t **model, size_t sizeNetwork,
                              lossFunctionType_t lossFunctionType, tensor_t *input, tensor_t *label,
                              trainingStats_t *trainingStats);

#endif // MODEL_API_H
