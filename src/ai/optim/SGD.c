#define SOURCE_FILE "AI_OPTIM_SGD"

#include "SGD.h"

SGDConfig_t *initSGDConfig(layerForwardBackward_t *model, size_t sizeModel, float lr,
float momentum, float weightDecay) {}

void SGDStep(SGDConfig_t *SGDConfig) {
    ;
}

void SGDZeroGrad(SGDConfig_t *SGDConfig) {
    ;
}

momentumBuffer_t *initMomentumBuffer(void *parameter) {}
