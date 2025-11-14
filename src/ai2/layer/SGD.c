#include "SGD.h"
#include "Tensor.h"
#include "Layer.h"


void initMomentumBuffer(momentumBuffer_t* momentumBuffer, tensor_t* dataTensor, tensor_t* gradTensor, float* momentums) {
    momentumBuffer->dataTensor = dataTensor;
    momentumBuffer->gradTensor = gradTensor;
    momentumBuffer->momentums = momentums;
}

uint32_t getNumberOfMomentumBuffersByLayerType(layerType_t type) {
    switch (type) {
    case LINEAR:
        return 2;
    case RELU:
        return 0;
    case CONV1D:
        return 2;
    default:
        return 0;
    }
}


void SGDStepFloat(SGDConfig_t *config) {
    for (size_t i = 0; i < config->sizeMomentumBuffers; i++) {
        tensor_t* dataTensor = config->momentum_buffer[i]->dataTensor;
        tensor_t* gradTensor = config->momentum_buffer[i]->gradTensor;
        float *momentum = config->momentum_buffer[i]->momentums;

        size_t numberOfElements = calcNumberOfElementsByTensor(dataTensor);

        for (size_t j = 0; j < numberOfElements; j++) {
            float grad = param->grad[j] + config->weightDecay * param->tensor->data[j];
            momentum[j] = config->momentum * momentum[j] + grad;
            param->tensor->data[j] -= config->lr * momentum[j];
        }
    }
}

void SGDStepAsym() {}

