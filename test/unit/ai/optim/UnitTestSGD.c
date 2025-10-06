#define SOURCE_FILE "SGD-UTEST"

#include "../../../../src/ai/nn/include/Linear.h"
#include "../../../../src/ai/nn/include/ReLU.h"
#include "AiHelpers.h"
#include "SGD.h"
#include "unity.h"

#include <stdlib.h>

void setUp() {}
void tearDown() {}

void unitTestInitMomentumBuffer() {
    float p[] = {0.f, 1.f, 2.f};
    parameter_t *param = initParameter(p, 3);
    momentumBuffer_t *momentumBuffer= initMomentumBuffer(param);

    float expectedMomentumBuffer[] = {0.f, 0.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedMomentumBuffer, momentumBuffer->momentums, 3);
    TEST_ASSERT_EQUAL_PTR(param, momentumBuffer->parameter);
}

void unitTestInitSGDConfig() {
    float w0[] = {0.f, 1.f, 2.f};
    float b0[] = {0.f, 1.f, -1.f};
    layerForwardBackward_t *linear0 = initLinearLayerForwardBackwardWithWeightBias(w0, 3, b0, 3);
    layerForwardBackward_t *relu0 = initReLULayerForwardBackward(3);
    layerForwardBackward_t *linear1 = initLinearLayerForwardBackwardWithWeightBias(w0, 3, b0, 3);
    layerForwardBackward_t *model[3] = {linear0, relu0, linear1};
    size_t sizeModel = sizeof(model)/sizeof(model[0]);
    float lr = 0.1f;
    float momentum = 0.9f;
    float weightDecay = 0.5f;
    SGDConfig_t *config = initSGDConfig(model, sizeModel, lr, momentum, weightDecay);

    linearConfig_t *linear0Conf = linear0->config;
    ReLUConfig_t *relu0Conf = relu0->config;
    linearConfig_t *linear1Conf = linear1->config;

    TEST_ASSERT_EQUAL_FLOAT(lr, config->lr);
    TEST_ASSERT_EQUAL_FLOAT(momentum, config->momentum);
    TEST_ASSERT_EQUAL_FLOAT(weightDecay, config->weightDecay);
    TEST_ASSERT_EQUAL_size_t(4, config->sizeMomentumBuffers);

    TEST_ASSERT_EQUAL_PTR(linear0Conf->weight, config->momentum_buffer[0]->parameter);
    TEST_ASSERT_EQUAL_PTR(linear0Conf->bias, config->momentum_buffer[1]->parameter);
    TEST_ASSERT_EQUAL_PTR(linear1Conf->weight, config->momentum_buffer[2]->parameter);
    TEST_ASSERT_EQUAL_PTR(linear1Conf->bias, config->momentum_buffer[3]->parameter);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear0Conf->weight->grad, config->momentum_buffer[0]->momentums, linear0Conf->weight->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear0Conf->bias->grad, config->momentum_buffer[1]->momentums, linear0Conf->bias->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear1Conf->weight->grad, config->momentum_buffer[2]->momentums, linear1Conf->weight->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear1Conf->bias->grad, config->momentum_buffer[3]->momentums, linear1Conf->bias->size);
}


void unitTestSGDStep() {
    float w0[] = {1.f, 2.f, -3.f};
    float w0Grad[] = {1.f, -1.f, 2.f};

    float b0[] = {-1.f, 3.f};
    float b0Grad[] = {1.f, 3.f};

    layerForwardBackward_t *linear0 = initLinearLayerForwardBackwardWithWeightBias(w0, 3, b0, 3);
    linearConfig_t *linearConf = linear0->config;
    linearConf->weight->grad = w0Grad;
    linearConf->bias->grad = b0Grad;
    layerForwardBackward_t *model[1] = {linear0};
    float lr = 0.1f;
    float momentum = 0.9f;
    float weightDecay = 0.01f;

    SGDConfig_t *config = initSGDConfig(model, 1, lr, momentum, weightDecay);

    SGDStep(config);

    float wPExpected[] = {0.899f, 2.098f, -3.197f};
    float bPExpected[] = {-1.099f, 2.697f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wPExpected, linearConf->weight->p, sizeof(wPExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY( bPExpected, linearConf->bias->p, sizeof(bPExpected)/sizeof(float));

    // Second Step with same grads but with momentum now
    SGDStep(config);

    float wPExpected2[] = {0.707201f, 2.284102f, -3.571103f};
    float bPExpected2[] = {-1.287001f, 2.121603f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wPExpected2, linearConf->weight->p,  sizeof(wPExpected2)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bPExpected2, linearConf->bias->p, sizeof(bPExpected2)/sizeof(float));
}

void unitTestSGDZeroGrad() {
    float w0[] = {1.f, 2.f, -3.f};
    float w0Grad[] = {1.f, -1.f, 2.f};

    float b0[] = {-1.f, 3.f};
    float b0Grad[] = {1.f, 3.f};

    layerForwardBackward_t *linear0 = initLinearLayerForwardBackwardWithWeightBias(w0, 3, b0, 3);
    linearConfig_t *linearConf = linear0->config;
    linearConf->weight->grad = w0Grad;
    linearConf->bias->grad = b0Grad;
    layerForwardBackward_t *model[1] = {linear0};
    float lr = 0.1f;
    float momentum = 0.9f;
    float weightDecay = 0.01f;

    SGDConfig_t *config = initSGDConfig(model, 1, lr, momentum, weightDecay);

    SGDZeroGrad(config);
    float wGradExpected[] = {0.f, 0.f, 0.f};
    float bGradExpected[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wGradExpected, w0Grad, sizeof(wGradExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bGradExpected, b0Grad, sizeof(bGradExpected)/sizeof(float));

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestSGDStep);
    RUN_TEST(unitTestSGDZeroGrad);
    RUN_TEST(unitTestInitMomentumBuffer);
    RUN_TEST(unitTestInitSGDConfig);
    UNITY_END();
}