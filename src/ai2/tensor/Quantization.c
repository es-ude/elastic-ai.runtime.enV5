#include "Quantization.h"

void initQuantization(qtype_t type, quantization_t *quantization) {
    switch (type) {
    case FLOAT32:
        quantization->type = FLOAT32;
        break;
    case INT32:
        quantization->type = INT32;
        break;
    case LINEAR:
        quantization->type = LINEAR;
        linearQ_t *linearConfig = quantization->qConfig;
        linearConfig->qMax = 255;
        break;
    default:
        break;
    }
}
