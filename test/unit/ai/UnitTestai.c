#define SOURCE_FILE "AI-UTEST"

#include <stdlib.h>

#include "Linear.h"
#include "MSE.h"
#include "ReLU.h"
#include "SGD.h"
#include "AiHelpers.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void unitTestNNTraining() {
    float input0[] = {1.f, 2.f, 3.f};
    float input1[] = {-1.f, 2.f, -3.f};
    float input2[] = {1.f, 2.f, 4.f};
    float input3[] = {1.f, 2.f, 5.f};
    float *inputs[] = {input0, input1, input2, input3};
    float label0[] = {0.f, 3.f};
    float label1[] = {1.f, 2.f};
    float label2[] = {1.f, 2.f};
    float label3[] = {1.f, 2.f};
    float *labels[] = {label0, label1, label2, label3};
    float weight0[] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    float bias0[] = {0.f, 0.f};
    layerForwardBackward_t *l0 = initLinearLayerForwardBackwardWithWeightBias(weight0, 6, bias0, 2);

    layerForwardBackward_t *model[1] = {l0};

    SGDConfig_t *sgdConfig = initSGDConfig(model, 1, 0.01f, 0.9f, 0.001f);


    float loss[2] = { 100.f, 100.f };
    int epochs = 0;
    while (abs((int)(loss[0]*100)) > 2 || abs((int)(loss[1]*100)) > 2) {
        loss[0] = 0.f;
        loss[1] = 0.f;
        for (int i=0; i<sizeof(inputs)/sizeof(inputs[0]); i++) {

            forward *fwd = layerFunctions[l0->type].forwardFunc;
            float *out = fwd(l0->config, inputs[i]);

            float *lossLocal = MSELossDOutput(out, labels[i], 2);
            loss[0] += lossLocal[0];
            loss[1] += lossLocal[1];

            backward *bwd = layerFunctions[l0->type].backwardFunc;
            float *propLoss = bwd(l0->config, loss, inputs[i]);

        }

        printf("%.6f \n", loss[0]);
        printf("%.6f \n", loss[1]);
        printf("%d \n", epochs);
        printf("\n");
        SGDStep(sgdConfig);
        SGDZeroGrad(sgdConfig);
        epochs += 1;
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestNNTraining);
    return UNITY_END();
}
