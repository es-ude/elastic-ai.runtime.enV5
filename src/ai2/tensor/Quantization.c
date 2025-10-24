#include "Quantization.h"
#include <stdio.h>

void initLinearQConfig(uint8_t qMax, roundingMode_t roundingMode, linearQConfig_t* linear_q) {
    linear_q->qMax = qMax;
    linear_q->roundingMode = roundingMode;
    linear_q->scale=1.f;
    linear_q->zeroPoint=(uint16_t)0;
}

void initInt32Quantization(quantization_t *quantization) {
    quantization->type = INT32;
    quantization->qConfig = NULL;
}

void initFloat32Quantization(quantization_t *quantization) {
    quantization->type = FLOAT32;
    quantization->qConfig = NULL;
}

void initLinearQuantization(linearQConfig_t * linear_q,quantization_t *quantization) {
    quantization->type = LINEAR;
    quantization->qConfig = linear_q;
}
