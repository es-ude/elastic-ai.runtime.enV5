#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "AiHelpers.h"
#include "Conv1d.h"

void setUp() {}
void tearDown() {}


void TEST_ASSERT_PARAMETERS_EQUAL(parameter_t *a, parameter_t *b) {
    TEST_ASSERT_EQUAL_size_t(a->size, b->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(a->p, b->p, a->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(a->grad, b->grad, a->size);
}



void unitTestInitConv1ConfigWithKernels() {
    size_t inputChannels = 2, outputChannels = 3, kernelSize = 4, stride = 5, dilation = 6, paddingSize = 9;
    float weight[24];
    for (int i = 0; i < 24; i++) {
        weight[i] = (float)i;
    }
    float bias[3] = {0.f, 0.f, 0.f};
    conv1dConfig_t *config = initConv1dConfigWithWeightAndBias(weight, bias, inputChannels, outputChannels, kernelSize, stride, dilation, ZEROS, paddingSize);

    TEST_ASSERT_EQUAL_size_t(inputChannels, config->inputChannels);
    TEST_ASSERT_EQUAL_size_t(outputChannels, config->outputChannels);
    TEST_ASSERT_EQUAL_size_t(kernelSize, config->kernelSize);
    TEST_ASSERT_EQUAL_size_t(stride, config->stride);
    TEST_ASSERT_EQUAL_size_t(dilation, config->dilation);
    TEST_ASSERT_EQUAL_size_t(paddingSize, config->padding->size);
    TEST_ASSERT_BIT_HIGH(0, config->useBias);

    parameter_t *expected_weight = initParameter(weight, sizeof(weight) / sizeof(weight[0]));
    parameter_t *expected_bias = initParameter(bias, sizeof(bias) / sizeof(bias[0]));

    TEST_ASSERT_PARAMETERS_EQUAL(expected_weight, config->weight);
    TEST_ASSERT_PARAMETERS_EQUAL(expected_bias, config->bias);
}

conv1dConfig_t *InitConv1dConfigSimple() {
    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 1, dilation = 1, paddingSize = 0;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f, //output channel 0
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f, //output channel 1
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f}; //output channel 2
    float bias[] = {0.f, 1.f, 2.f};
    conv1dConfig_t *config = initConv1dConfigWithWeightAndBias(weight, bias, inputChannels, outputChannels, kernelSize, stride, dilation, NONE, paddingSize);
    return config;
}

conv1dConfig_t *initConv1dConfigComplex() {
    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 3, dilation = 2, paddingSize = 1;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f, //output channel 0
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f, //output channel 1
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f}; //output channel 2
    float bias[] = {0.f, 1.f, 2.f};
    conv1dConfig_t *config = initConv1dConfigWithWeightAndBias(weight, bias, inputChannels, outputChannels, kernelSize, stride, dilation, ZEROS, paddingSize);
    return config;
}

void unitTestCalcOutputSizeForInputSizeAndConv1dConfigSimple() {
    conv1dConfig_t *config = InitConv1dConfigSimple();
    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(32, config);
    size_t expectedOutputSize = 14*3;
    TEST_ASSERT_EQUAL_size_t(expectedOutputSize, outputSize);
}

void unitTestCalcOutputSizeForInputSizeAndConv1dConfigComplex() {
    conv1dConfig_t *config = initConv1dConfigComplex();
    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(12, config);
    size_t expectedOutputSize = 6;
    TEST_ASSERT_EQUAL_size_t(expectedOutputSize, outputSize);

    size_t outputSize1 = calcOutputSizeForInputSizeAndConv1dConfig(32, config);
    size_t expectedOutputSize1 = 15;
    TEST_ASSERT_EQUAL_size_t(expectedOutputSize1, outputSize1);

    size_t outputSize2 = calcOutputSizeForInputSizeAndConv1dConfig(34, config);
    size_t expectedOutputSize2 = 15;
    TEST_ASSERT_EQUAL_size_t(expectedOutputSize2, outputSize2);

    size_t outputSize3 = calcOutputSizeForInputSizeAndConv1dConfig(36, config);
    size_t expectedOutputSize3 = 18;
    TEST_ASSERT_EQUAL_size_t(expectedOutputSize3, outputSize3);
}

void unitTestConv1dForward() {
    conv1dConfig_t *config = InitConv1dConfigSimple();
    float input[12] = {
        1.f, 2.f, 3.f, 4.f, 5.f, 6.f,
        -1.f, -2.f, -3.f, -4.f ,-5.f, -6.f
    };
    float *output = conv1dForward(config, input);
    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(sizeof(input)/sizeof(input[0]), config);

    float expectedOutput[12] = {-10.f, -14.f, -18.f, -22.f, 8.f,  10.f,  12.f,  14.f, -3.f,  -4.f,  -5.f,  -6.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedOutput, output, outputSize);
}

void unitTestConv1dBackward() {
    conv1dConfig_t *config = InitConv1dConfigSimple();
    float input[12] = {
        1.f, 2.f, 3.f, 4.f, 5.f, 6.f,
        -1.f, -2.f, -3.f, -4.f ,-5.f, -6.f
    };
    float loss[12] = {-1.f, 0.f, -2.f, -1.f, 1.f,  0.f,  -2.f,  -4.f, -2.f,  -1.f,  0.f,  -1.f};
    float *propagatedLoss = conv1dBackward(config, loss, input);

    float expectedPropagatedLoss[12] = {-0.8333f, -0.6667f, -1.0000f, -0.8333f,  0.1667f, -0.1667f, -1.3333f, -0.3333f,  0.6667f,  0.3333f, -0.1667f, -0.8333f};

    float expectedWeightGrad[18] = {1.8333f,  2.5000f,  3.1667f, -1.8333f, -2.5000f, -3.1667f, 3.5000f,  4.3333f,  5.1667f, -3.5000f, -4.3333f, -5.1667f, 1.3333f,  2.0000f,  2.6667f, -1.3333f, -2.0000f, -2.6667f};

    float expectedBiasGrad[3] = {0.6667f, 0.8333f, 0.6667f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedPropagatedLoss, propagatedLoss, sizeof(expectedPropagatedLoss)/sizeof(expectedPropagatedLoss[0]));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedWeightGrad, expectedWeightGrad, sizeof(expectedWeightGrad)/sizeof(expectedWeightGrad[0]));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedBiasGrad, expectedBiasGrad, sizeof(expectedBiasGrad)/sizeof(expectedBiasGrad[0]));

    float *propagatedLoss2 = conv1dBackward(config, loss, input);

    float expectedPropagatedLoss2[12] = {-0.8333f, -0.6667f, -1.0000f, -0.8333f,  0.1667f, -0.1667f, -1.3333f, -0.3333f,  0.6667f,  0.3333f, -0.1667f, -0.8333f};

    float expectedWeightGrad2[18] = {3.6667f, 5.0000f, 6.3333f, -3.6667f, -5.0000f, -6.3333f, 7.0000f, 8.6667f, 10.3333f, -7.0000f,  -8.6667f, -10.3333f, 2.6667f, 4.0000f, 5.3333f, -2.6667f,  -4.0000f,  -5.3333f};

    float expectedBiasGrad2[3] = {1.3333f, 1.6667f, 1.3333f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedPropagatedLoss2, propagatedLoss2, sizeof(expectedPropagatedLoss2)/sizeof(expectedPropagatedLoss2[0]));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedWeightGrad2, expectedWeightGrad2, sizeof(expectedWeightGrad2)/sizeof(expectedWeightGrad2[0]));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedBiasGrad2, expectedBiasGrad2, sizeof(expectedBiasGrad2)/sizeof(expectedBiasGrad2[0]));

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitConv1ConfigWithKernels);
    RUN_TEST(unitTestCalcOutputSizeForInputSizeAndConv1dConfigSimple);
    RUN_TEST(unitTestCalcOutputSizeForInputSizeAndConv1dConfigComplex);
    RUN_TEST(unitTestConv1dForward);
    UNITY_END();
}