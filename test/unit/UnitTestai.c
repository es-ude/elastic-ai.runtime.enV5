#define SOURCE_FILE "Linear-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "ai.h"
//#include "linear.h"

void setUp() {
    ;
}

void tearDown() {
    ;
}

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

// MSELoss derived to the output = Element-wise Output-Label
void unitTestMSELossDOutput() {
    float label[] = {2.f, 3.f, 4.f};
    float output[] = {-2.f, 1.f, 3.f};
    float* lossToPropagate=MSELossDOutput(output, label, sizeof(output)/sizeof(float));
    float expectedLossToPropagate[] = {4.f, -2.f, -1.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedLossToPropagate, lossToPropagate, sizeof(expectedLossToPropagate)/sizeof(float));
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

    float expected_propagated_loss[] = {-6.f, -27.f, -36.f};

    float expected_weight_grad[] = {0.f, 6.f, -12.f, 0.f, -3.f, -6.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagated_loss, sizeof(expected_propagated_loss)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig.weight.grad, linearConfig.weight.size);
}


int main() {
    UNITY_BEGIN();

    RUN_TEST(unitTestLinearForward);
    RUN_TEST(unitTestLinearBackward);

    return UNITY_END();
}
