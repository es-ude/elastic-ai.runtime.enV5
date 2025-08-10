#define SOURCE_FILE "AI_OPTIM_SGD"

#include "SGD.h"


void SGDStep(SGDConfig_t *config) {
    for (size_t i = 0; i < config->sizeMomentumBuffers; ++i) {
        parameter_t *param = config->momentum_buffer[i].parameter;
        float *momentum = config->momentum_buffer[i].momentums;

        for (size_t j = 0; j < param->size; ++j) {
            float grad = param->grad[j] + config->weightDecay * param->p[j];
            momentum[j] = config->momentum * momentum[j] + grad;
            param->p[j] -= config->lr * momentum[j];
        }
    }
}

void SGDZeroGrad(SGDConfig_t *config) {
    for (size_t i = 0; i < config->sizeMomentumBuffers; ++i) {
        parameter_t *param = config->momentum_buffer[i].parameter;
        for (size_t j = 0; j < param->size; ++j) {
            param->grad[j] = 0.0f;
        }
    }
}
