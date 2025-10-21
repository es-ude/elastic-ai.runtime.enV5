#include "Quantization.h"

#include <stdlib.h>

quantization_t *initQuantization(dtype_t type) {
    quantization_t *quantization = calloc(1, sizeof(quantization_t));

    switch (type) {
    case FLOAT32:
        quantization->type = FLOAT32;
        return quantization;
    case INT32:
        quantization->type = INT32;
        return quantization;
    case LINEAR:
        quantization->type = LINEAR;
        quantization->qConfig = calloc(1, sizeof(linearQ_t));
        linearQ_t *linearConfig = quantization->qConfig;
        linearConfig->qMax = 255;
        return quantization;
    default:
        return NULL;
    }
}
