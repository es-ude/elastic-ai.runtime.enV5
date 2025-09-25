#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"
#include "AiHelpers.h"
#include "Linear.h"

void setUp() {}
void tearDown() {}

linearConfig_t *initLinearConfig() {
    size_t weightSize = 6;
    size_t biasSize = 2;
    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float bias[] = {-1.f, 3.f};
    linearConfig_t *config = initLinearConfigWithWeightBias(weight, weightSize, bias, biasSize);

    return config;
}

void unitTestLinearForward() {
    linearConfig_t *linearConfig = initLinearConfig();

    float input[] = {0.f, 1.f, 2.f};
    float *output = linearForward(linearConfig, input);
    float expected_result[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, output, linearConfig->outputSize);
}

void unitTestLinearBackward() {
    linearConfig_t *linearConfig = initLinearConfig();

    float input[] = {0.f, 1.f, 2.f};

    float loss[] = {-4.f, -3.f};

    float *propagated_loss = linearBackward(linearConfig, loss, input);

    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagated_loss,
                                  sizeof(expected_propagated_loss) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig->weight->grad,
                                  linearConfig->weight->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig->bias->grad, linearConfig->bias->size);

    float *propagated_loss_2 = linearBackward(linearConfig, loss, input);

    float expected_propagated_loss_2[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad_2[] = {0.f, -8.f, -16.f, 0.f, -6.f, -12.f};

    float expected_bias_grad_2[] = {-8.f, -6.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss_2, propagated_loss_2,
                              sizeof(expected_propagated_loss_2) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad_2, linearConfig->weight->grad,
                                  linearConfig->weight->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad_2, linearConfig->bias->grad, linearConfig->bias->size);
}

void unitTestInitLinearConfigWithWeightBias() {
    size_t weightSize = 4;
    size_t biasSize = 2;
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    float bias[] = {-1.f, -2.f};

    linearConfig_t *linearConfig =
        initLinearConfigWithWeightBias(weights, weightSize, bias, biasSize);

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_size_t(weightSize / biasSize, linearConfig->inputSize);
    TEST_ASSERT_EQUAL_size_t(biasSize, linearConfig->outputSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->p, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->p, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

void unitTestInitLinearForwardWithWeightBias() {
    size_t weightSize = 4;
    size_t biasSize = 2;
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    float bias[] = {-1.f, -2.f};

    layerForward_t *linearLayerForward =
        initLinearLayerForwardWithWeightBias(weights, weightSize, bias, biasSize);
    layerType_t layerType = LINEAR;
    TEST_ASSERT_EQUAL(layerType, linearLayerForward->type);
    TEST_ASSERT_EQUAL_PTR(linearForward, layerFunctions[linearLayerForward->type].forwardFunc);
    linearConfig_t *linearConfig = linearLayerForward->config;

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_size_t(weightSize / biasSize, linearConfig->inputSize);
    TEST_ASSERT_EQUAL_size_t(biasSize, linearConfig->outputSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->p, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->p, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

void unitTestInitLinearForwardBackwardWithWeightBias() {
    size_t weightSize = 4;
    size_t biasSize = 2;
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    float bias[] = {-1.f, -2.f};

    layerForwardBackward_t *linearLayerForwardBackward =
        initLinearLayerForwardBackwardWithWeightBias(weights, weightSize, bias, biasSize);
    layerType_t layerType = LINEAR;
    TEST_ASSERT_EQUAL(layerType, linearLayerForwardBackward->type);
    TEST_ASSERT_EQUAL(&linearForward, layerFunctions[linearLayerForwardBackward->type].forwardFunc);
    TEST_ASSERT_EQUAL(&linearBackward, layerFunctions[linearLayerForwardBackward->type].backwardFunc);
    linearConfig_t *linearConfig = linearLayerForwardBackward->config;

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_size_t(weightSize / biasSize, linearConfig->inputSize);
    TEST_ASSERT_EQUAL_size_t(biasSize, linearConfig->outputSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->p, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->p, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

void unitTestLinearForwardAutomatic() {
    float input[] = {1.f, 2.f};
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    float bias[] = {-1.f, -2.f};
    layerForward_t *l = initLinearLayerForwardWithWeightBias(weights, 4, bias, 2);
    forward *fwd = layerFunctions[l->type].forwardFunc;
    float *out = fwd(l->config, input);
    float expectedOut[] = {4.f, 9.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedOut, out, 2);
}

void unitTestLinearForwardBackwardAutomatic() {
    float input[] = {0.f, 1.f, 2.f};

    float loss[] = {-4.f, -3.f};
    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float bias[] = {-1.f, 3.f};

    layerForwardBackward_t *l = initLinearLayerForwardBackwardWithWeightBias(weight,6,bias,2);
    forward *forward = layerFunctions[l->type].forwardFunc;
    float *result = forward(l->config, input);
    float expected_result[] = {-5.f, -4.f};;

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, result, 2);
    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    backward *backward = layerFunctions[l->type].backwardFunc;
    float *propagatedLoss = backward(l->config, loss, input);

    linearConfig_t *linearConfig = l->config;

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagatedLoss,
                              sizeof(expected_propagated_loss) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig->weight->grad,
                                  linearConfig->weight->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig->bias->grad, linearConfig->bias->size);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestLinearForward);
    RUN_TEST(unitTestLinearBackward);
    RUN_TEST(unitTestInitLinearConfigWithWeightBias);
    RUN_TEST(unitTestInitLinearForwardWithWeightBias);
    RUN_TEST(unitTestInitLinearForwardBackwardWithWeightBias);
    RUN_TEST(unitTestLinearForwardAutomatic);
    RUN_TEST(unitTestLinearForwardBackwardAutomatic);
    return UNITY_END();
}
