#ifndef SEQUENTIAL_H
#define SEQUENTIAL_H
#include "Layer.h"
#include "Tensor.h"

typedef struct trainingStats {
    tensor_t *output;
    tensor_t *loss;
} trainingStats_t;

void sequentialForward(layer_t **model, size_t numberOfLayers, tensor_t *input, tensor_t *output);

void sequentialCalculateGrads(layer_t **model, size_t sizeNetwork,
                              lossFunctionType_t lossFunctionType, tensor_t *input, tensor_t *label,
                              trainingStats_t *trainingStats);

#endif // SEQUENTIAL_H
