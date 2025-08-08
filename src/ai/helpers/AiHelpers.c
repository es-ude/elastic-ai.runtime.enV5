#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"

#include <string.h>

parameter_t *initParameter(float *p, size_t size) {
    parameter_t *param = calloc(1, sizeof(parameter_t));

    param->p = calloc(size, sizeof(float));
    memcpy(param->p, p, size * sizeof(float));

    param->size = size;
    param->grad = calloc(size, sizeof(float));
    return param;
}



float *sequentialForward(layerForward_t network[], float *input) {
    ;
}


void sequentialCalculateGrads(layerForwardBackward_t network[], void *lossFunction, float *input) {
    ;
}