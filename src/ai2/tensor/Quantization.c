#include "Quantization.h"
#include <stdio.h>

void initAsymQConfig(uint8_t qBits, roundingMode_t roundingMode, asymQConfig_t *asymQConfig) {
    asymQConfig->qBits = qBits;
    asymQConfig->roundingMode = roundingMode;
    asymQConfig->scale = 0.f;
    asymQConfig->zeroPoint = (uint16_t)0;
}

void initInt32Quantization(quantization_t *quantization) {
    quantization->type = INT32;
    quantization->qConfig = NULL;
}

void initFloat32Quantization(quantization_t *quantization) {
    quantization->type = FLOAT32;
    quantization->qConfig = NULL;
}

void initAsymQuantization(asymQConfig_t *asymQConfig, quantization_t *quantization) {
    quantization->type = ASYM;
    quantization->qConfig = asymQConfig;
}
