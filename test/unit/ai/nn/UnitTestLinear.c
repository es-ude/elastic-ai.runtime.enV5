#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "AiHelpers.h"
#include "Linear.h"

void setUp(){}
void tearDown(){}

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
    float *output = linearForward(linearConfig, input);

    float loss[] = {-4.f, -3.f};

    float *propagated_loss = linearBackward(linearConfig, loss, input);

    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagated_loss, sizeof(expected_propagated_loss)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig->weight->grad, linearConfig->weight->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(loss, linearConfig->bias->grad, linearConfig->bias->size);
}

void unitTestInitLinearConfigWithWeightBias() {
    size_t weightSize = 4;
    size_t biasSize = 2;
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    float bias[] = {-1.f, -2.f};

    linearConfig_t *linearConfig = initLinearConfigWithWeightBias(weights, weightSize, bias, biasSize);

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_size_t(weightSize/biasSize, linearConfig->inputSize);
    TEST_ASSERT_EQUAL_size_t(biasSize, linearConfig->outputSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->p, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->p, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestLinearForward);
    RUN_TEST(unitTestLinearBackward);
    RUN_TEST(unitTestInitLinearConfigWithWeightBias);
    UNITY_END();
}