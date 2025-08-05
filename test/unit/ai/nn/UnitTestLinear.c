#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "AiHelpers.h"
#include "Linear.h"

void setUp(){}
void tearDown(){}


void unitTestLinearForward() {
    parameter_t weight;
    float wP[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float wGrad[6];
    weight.p = wP;
    weight.grad = wGrad;
    weight.size = sizeof(wP)/sizeof(float);

    parameter_t bias;
    float bP[] = {-1.f, 3.f};
    float bGrad[2];
    bias.p = bP;
    bias.grad = bGrad;
    bias.size = sizeof(bP)/sizeof(float);

    linearConfig_t linearConfig;
    linearConfig.inputSize = weight.size/bias.size;
    linearConfig.outputSize = bias.size;
    linearConfig.weight = weight;
    linearConfig.bias = bias;

    float input[] = {0.f, 1.f, 2.f};
    float *output = linearForward(&linearConfig, input);

    float expected_result[] = {-5.f, -4.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, output, linearConfig.outputSize);
}

void unitTestLinearBackward() {
    parameter_t weight;
    float wP[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float wGrad[6];
    weight.p = wP;
    weight.grad = wGrad;
    weight.size = sizeof(wP)/sizeof(float);

    parameter_t bias;
    float bP[] = {-1.f, 3.f};
    float bGrad[2];
    bias.p = bP;
    bias.grad = bGrad;
    bias.size = sizeof(bP)/sizeof(float);

    linearConfig_t linearConfig;
    linearConfig.inputSize = weight.size/bias.size;
    linearConfig.outputSize = bias.size;
    linearConfig.weight = weight;
    linearConfig.bias = bias;

    float input[] = {0.f, 1.f, 2.f};
    float *output = linearForward(&linearConfig, input);

    float loss[] = {-4.f, -3.f};

    float *propagated_loss = linearBackward(&linearConfig, loss, input);

    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    // weight.grad[0] = loss[0] * input[0]
    // weight.grad[1] = loss[0] * input[1]
    // weight.grad[2] = loss[0] * input[2]
    // weight.grad[3] = loss[1] * input[0]
    // weight.grad[4] = loss[1] * input[1]
    // weight.grad[5] = loss[1] * input[2]


    // prop_loss[0] = wP[0]*loss[0] + wP[3]*loss[1]
    // prop_loss[1] = wP[1]*loss[0] + wP[4]*loss[1]
    // prop_loss[2] = wP[2]*loss[0] + wP[5]*loss[1]

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagated_loss, sizeof(expected_propagated_loss)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig.weight.grad, linearConfig.weight.size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(loss, linearConfig.bias.grad, linearConfig.bias.size);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestLinearForward);
    RUN_TEST(unitTestLinearBackward);
    UNITY_END();
}