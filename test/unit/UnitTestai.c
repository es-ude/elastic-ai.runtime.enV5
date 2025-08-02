#define SOURCE_FILE "Linear-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "ai.h"


void setUp() {
    ;
}

void tearDown() {
    ;
}

void unitTestInitParameter() {
    const float p[] = {1.f, 2.f, 3.f};
    const size_t size = sizeof(p) / sizeof(p[0]);

    const parameter_t *parameter = initParameter(p, size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(p, parameter->p, size);

    const float expectedGrad[] = {0.f, 0.f, 0.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedGrad, parameter->grad, size);

    TEST_ASSERT_EQUAL_size_t(size, parameter->size);
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

void unitTestReLUForward() {
    float input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};

    ReLUConfig_t config;
    config.size = sizeof(input)/sizeof(float);

    float *result = ReLUForward(&config, input);
    float expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, result, config.size);
}

void unitTestReLUBackward() {
    float input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    float grad[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};

    ReLUConfig_t config;
    config.size = sizeof(input)/sizeof(float);

    float *result = ReLUBackward(&config, grad, input);
    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, result, config.size);
}

void unitTestMSELossDOutput() {
    float output[] = {1.f, 2.f, -3.f};
    float label[] = {-5.f, -4.f, 2.f};
    float *dLossOutput = MSELossDOutput(output, label, sizeof(output)/sizeof(float));
    float expected[] = {6.f, 6.f, -5.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, dLossOutput, sizeof(output)/sizeof(float));
}

void unitTestSGDStep() {
    float wP[] = {1.f, 2.f, -3.f};
    float wGrad[] = {1.f, -1.f, 2.f};
    float wMomentum[] = {0.f, 0.f, 0.f}; //zeros size of P

    float bP[] = {-1.f, 3.f};
    float bGrad[] = {1.f, 3.f};
    float bMomentum[] = {0.f, 0.f}; //zeros size of P

    parameter_t weight;
    weight.p = wP;
    weight.grad = wGrad;
    weight.size = sizeof(wP)/sizeof(float);

    parameter_t bias;
    bias.p = bP;
    bias.grad = bGrad;
    bias.size = sizeof(bP)/sizeof(float);

    momentumBuffer_t wMomentumBuffer;
    wMomentumBuffer.parameter = &weight;
    wMomentumBuffer.momentums = wMomentum;

    momentumBuffer_t bMomentumBuffer;
    bMomentumBuffer.parameter = &bias;
    bMomentumBuffer.momentums = bMomentum;

    momentumBuffer_t *momentumBuffer[2];
    momentumBuffer[0] = &wMomentumBuffer;
    momentumBuffer[1] = &bMomentumBuffer;

    SGDConfig_t config;
    config.lr = 0.1f;
    config.momentum = 0.9f;
    config.weightDecay = 0.01f;
    config.momentum_buffer = momentumBuffer;
    config.sizeMomentumBuffers = sizeof(momentumBuffer)/sizeof(momentumBuffer_t);

    SGDStep(&config);

    float wPExpected[] = {0.899f, 2.098f, -3.197f};
    float bPExpected[] = {-1.099f, 2.697f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wP, wPExpected, sizeof(wPExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bP, bPExpected, sizeof(bPExpected)/sizeof(float));

    //Second Step with same grads but with momentum now
    SGDStep(&config);

    float wPExpected2[] = {0.82708f, 1.06984f, -2.05876f};
    float bPExpected2[] = {0.98892f, 2.48124f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wP, wPExpected2, sizeof(wPExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bP, bPExpected2, sizeof(bPExpected)/sizeof(float));

}

void unitTestSGDZeroGrad() {
    float wP[] = {1.f, 2.f, -3.f};
    float wGrad[] = {1.f, -1.f, 2.f};
    float wMomentum[] = {0.f, 0.f, 0.f}; //zeros size of P

    float bP[] = {-1.f, 3.f};
    float bGrad[] = {1.f, 3.f};
    float bMomentum[] = {0.f, 0.f}; //zeros size of P

    parameter_t weight;
    weight.p = wP;
    weight.grad = wGrad;
    weight.size = sizeof(wP)/sizeof(float);

    parameter_t bias;
    bias.p = bP;
    bias.grad = bGrad;
    bias.size = sizeof(bP)/sizeof(float);

    momentumBuffer_t wMomentumBuffer;
    wMomentumBuffer.parameter = &weight;
    wMomentumBuffer.momentums = wMomentum;

    momentumBuffer_t bMomentumBuffer;
    bMomentumBuffer.parameter = &bias;
    bMomentumBuffer.momentums = bMomentum;

    momentumBuffer_t *momentumBuffer[2];
    momentumBuffer[0] = &wMomentumBuffer;
    momentumBuffer[1] = &bMomentumBuffer;

    SGDConfig_t config;
    config.lr = 0.1f;
    config.momentum = 0.9f;
    config.weightDecay = 0.01f;
    config.momentum_buffer = momentumBuffer;
    config.sizeMomentumBuffers = sizeof(momentumBuffer) / sizeof(momentumBuffer_t);

    SGDZeroGrad(&config);
    float wGradExpected[] = {0.f, 0.f, 0.f};
    float bGradExpected[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wGrad, wGradExpected, sizeof(wGradExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bGrad, bGradExpected, sizeof(bGradExpected)/sizeof(float));

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitParameter);
    //RUN_TEST(unitTestLinearForward);
    //RUN_TEST(unitTestLinearBackward);
    //RUN_TEST(unitTestReLUForward);
    //RUN_TEST(unitTestReLUBackward);
    //RUN_TEST(unitTestMSELossDOutput);
    //RUN_TEST(unitTestSGDStep);
    return UNITY_END();
}