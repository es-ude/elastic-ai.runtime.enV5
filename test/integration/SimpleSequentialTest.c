#include "Tensor.h"
#include "Layer.h"
#include "Linear.h"

typedef struct sequential sequential_t;

struct sequential {
    layer_t currentLayer;
    void (append)(sequential_t* self, layer_t *otherLayer);
    int (getSize)(const sequential_t* self);
};

int main(void) {
    layer_t linearLayer;

    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float weightGrads[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;

    quantization_t weightQ = {
        .type = FLOAT32
    };
    quantization_t weightGradQ = {
        .type = FLOAT32
    };

    parameter_t weights = {
        .data = weightData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightQ,
        .gradQuantization = &weightGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };

    float biasData[] = {-1.f, 3.f};
    float biasGrads[] = {0.f, 0.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 1;

    quantization_t biasQ = {
        .type = FLOAT32
    };
    quantization_t biasGradQ = {
        .type = FLOAT32
    };

    parameter_t bias = {
        .data = biasData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasQ,
        .gradQuantization = &biasGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };

    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, FLOAT32, &weights, &bias);

    linearLayer.type = LINEAR;
    linearLayer.layerConfig = &linearConfig;
    linearLayer.calcOutputShape = calcOutputShape;
    linearLayer.forward = linearForward;
    linearLayer.backward = NULL;

    sequential_t sequential;
    sequential.currentLayer = linearLayer;

    forwardSequential()
}