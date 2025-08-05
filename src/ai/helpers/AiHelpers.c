#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"

parameter_t *initParameter(float *p, size_t size) {
    parameter_t *param = calloc(1, sizeof(parameter_t));
    param->p = p;
    param->size = size;

    param->grad = calloc(size, sizeof(float));
    for (int i = 0; i < size; i++) {
        param->grad[i] = 0;
    }

    return param;
}

float *sequentialForward(layerForward_t network[], float *input) {
    ;
}


void sequentialCalculateGrads(layerForwardBackward_t network[], void *lossFunction, float *input) {
    ;
}