#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "AiHelpers.h"
#include "Conv1d.h"
#include "MSE.h"

#include <string.h>

void setUp() {}
void tearDown() {}

conv1dConfig_t *InitConv1dConfigSimple() {
    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 1, dilation = 1,
           paddingSize = 0;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f, //output channel 0
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f, //output channel 1
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f}; //output channel 2

    tensor_t weightTensor = {
        .data = weight,
        .numberOfDimensions = 2,
        .dimensions = (size_t[]){3, 6}
    };

    float bias[] = {0.f, 1.f, 2.f};

    tensor_t biasTensor = {
        .data = bias,
        .numberOfDimensions = 1,
        .dimensions = (size_t[]){3}
    };
    conv1dConfig_t *config = initConv1dConfigWithWeightAndBias(
        weightTensor, biasTensor, inputChannels, outputChannels, kernelSize, stride, dilation, NONE,
        paddingSize);
    return config;
}

conv1dConfig_t *initConv1dConfigComplex() {
    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 3, dilation = 2,
           paddingSize = 1;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f, //output channel 0
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f, //output channel 1
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f}; //output channel 2

    tensor_t weightTensor = {
        .data = weight,
        .numberOfDimensions = 2,
        .dimensions = (size_t[]){3, 6}
    };

    float bias[] = {0.f, 1.f, 2.f};
    tensor_t biasTensor = {
        .data = bias,
        .numberOfDimensions = 1,
        .dimensions = (size_t[]){3}
    };

    conv1dConfig_t *config = initConv1dConfigWithWeightAndBias(
        weightTensor, biasTensor, inputChannels, outputChannels, kernelSize, stride, dilation,
        ZEROS,
        paddingSize);
    return config;
}

void TEST_ASSERT_PARAMETERS_EQUAL(parameter_t *a, parameter_t *b) {
    TEST_ASSERT_EQUAL_size_t(a->size, b->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(a->p, b->p, a->size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(a->grad, b->grad, a->size);
}


void unitTestInitConv1Config() {
    size_t inputChannels = 2, outputChannels = 3, kernelSize = 4, stride = 5, dilation = 6,
           paddingSize = 9;
    float weight[24];
    for (int i = 0; i < 24; i++) {
        weight[i] = (float)i;
    }
    size_t weightDims[] = {3, 6};
    tensor_t weightTensor;
    weightTensor.data = weight;
    weightTensor.numberOfDimensions = sizeof(weightDims) / sizeof(size_t);
    weightTensor.dimensions = weightDims;

    float bias[3] = {0.f, 0.f, 0.f};
    size_t biasDims[] = {3};
    tensor_t biasTensor;
    biasTensor.data = bias;
    biasTensor.numberOfDimensions = sizeof(biasDims) / sizeof(size_t);
    biasTensor.dimensions = biasDims;
    conv1dConfig_t *config = initConv1dConfigWithWeightAndBias(
        weightTensor, biasTensor, inputChannels, outputChannels, kernelSize, stride, dilation,
        ZEROS,
        paddingSize);

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


void unitTestCalcOutputSizeForInputSizeAndConv1dConfigSimple() {
    conv1dConfig_t *config = InitConv1dConfigSimple();
    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(32, config);
    size_t expectedOutputSize = 14 * 3;
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
        1.f, 2.f, 3.f, 4.f, 5.f, 6.f, // inputChannel 0
        -1.f, -2.f, -3.f, -4.f, -5.f, -6.f // inputChannel 1
    };

    tensor_t inputTensor = {
        .data = input,
        .numberOfDimensions = 2,
        .dimensions = (size_t[]){2, 6}
    };

    float *output = conv1dForward(config, &inputTensor);

    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(
        sizeof(input) / sizeof(input[0]), config);

    float expectedOutput[12] = {
        -10.f, -14.f, -18.f, -22.f, // outputChannel 0
        8.f, 10.f, 12.f, 14.f, // outputChannel 1
        -3.f, -4.f, -5.f, -6.f // outputChannel 2
    };
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedOutput, output, outputSize);
}

void unitTestConv1dForwardComplex(void) {
    conv1dConfig_t *config = initConv1dConfigComplex();
    float input[12] = {
        1.f, 2.f, 3.f, 4.f, 5.f, 6.f, // inputChannel 0
        -1.f, -2.f, -3.f, -4.f, -5.f, -6.f // inputChannel 1
    };
    tensor_t inputTensor = {
        .data = input,
        .numberOfDimensions = 2,
        .dimensions = (size_t[]){2, 6}
    };
    float *output = conv1dForward(config, &inputTensor);
    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(
        sizeof(input) / sizeof(input[0]), config);

    float expectedOutput[6] = {
        -12.f, -10.f, // outputChannel 0
        11.f, 3.f, // outputChannel 1
        -6.f, 5.f // outputChannel 2
    };

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedOutput, output, outputSize);
}

void unitTestConv1dBackward() {
    conv1dConfig_t *config = InitConv1dConfigSimple();
    float input[] = {
        1.f, 2.f, 3.f, 4.f, 5.f, 6.f,
        -1.f, -2.f, -3.f, -4.f, -5.f, -6.f
    };
    size_t inputDims[] = {2, 6};
    tensor_t *inputTensor = initTensor(input, 2, inputDims);

    float delta[] = {-1.f, 0.f, -2.f, -1.f,
                     1.f, 0.f, -2.f, -4.f,
                     -2.f, -1.f, 0.f, -1.f};
    size_t deltaDims[] = {3, 4};
    tensor_t *deltaTensor = initTensor(delta, 2, deltaDims);

    float conv1dForwardOutput[] = {
        -10.f, -14.f, -18.f, -22.f, // outputChannel 0
        8.f, 10.f, 12.f, 14.f, // outputChannel 1
        -3.f, -4.f, -5.f, -6.f // outputChannel 2
    };
    size_t conv1dOutputDims[] = {3, 4};
    tensor_t *conv1dOutputTensor = initTensor(conv1dForwardOutput, 2, conv1dOutputDims);

    float target[12];
    for (size_t i = 0; i < 12; i++) {
        target[i] = conv1dForwardOutput[i] + deltaTensor->data[i];
    }
    size_t targetDims[] = {3, 4};
    tensor_t *targetTensor = initTensor(target, 2, targetDims);

    size_t inputSize = sizeof(input) / sizeof(input[0]);
    tensor_t *gradOut = MSELossBackward(conv1dOutputTensor, targetTensor);

    tensor_t *propagatedLoss1 = conv1dBackward(config, gradOut, inputTensor);

    float expectedPropagatedLoss1[] = {-0.8333f, -0.6667f, -1.0000f, -0.8333f, 0.1667f,
                                       -0.1667f,
                                       -1.3333f, -0.3333f, 0.6667f, 0.3333f, -0.1667f,
                                       -0.8333f};

    float expectedWeightGrad1[18] = {1.8333f, 2.5000f, 3.1667f, -1.8333f, -2.5000f, -3.1667f,
                                     3.5000f, 4.3333f, 5.1667f, -3.5000f, -4.3333f, -5.1667f,
                                     1.3333f, 2.0000f, 2.6667f, -1.3333f, -2.0000f, -2.6667f};

    float expectedBiasGrad1[3] = {0.6667f, 0.8333f, 0.6667f};

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedPropagatedLoss1[i], propagatedLoss1->data[i]);
    }
    for (size_t i = 0; i < config->weight->size; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedWeightGrad1[i], config->weight->grad[i]);
    }
    for (size_t i = 0; i < config->bias->size; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedBiasGrad1[i], config->bias->grad[i]);
    }

    tensor_t *propagatedLoss2 = conv1dBackward(config, gradOut, inputTensor);

    float expectedPropagatedLoss2[] = {-0.8333f, -0.6667f, -1.0000f, -0.8333f, 0.1667f,
                                       -0.1667f,
                                       -1.3333f, -0.3333f, 0.6667f, 0.3333f, -0.1667f,
                                       -0.8333f};

    float expectedWeightGrad2[18] = {3.6667f, 5.0000f, 6.3333f, -3.6667f, -5.0000f, -6.3333f,
                                     7.0000f, 8.6667f, 10.3333f, -7.0000f, -8.6667f, -10.3333f,
                                     2.6667f, 4.0000f, 5.3333f, -2.6667f, -4.0000f, -5.3333f};

    float expectedBiasGrad2[3] = {1.3333f, 1.6667f, 1.3333f};

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedPropagatedLoss2[i], propagatedLoss2->data[i]);
    }
    for (size_t i = 0; i < config->weight->size; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedWeightGrad2[i], config->weight->grad[i]);
    }
    for (size_t i = 0; i < config->bias->size; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedBiasGrad2[i], config->bias->grad[i]);
    }
}


void unitTestConv1dBackwardComplex(void) {
    conv1dConfig_t *config = initConv1dConfigComplex();
    float input[12] = {
        1.f, 2.f, 3.f, 4.f, 5.f, 6.f,
        -1.f, -2.f, -3.f, -4.f, -5.f, -6.f
    };
    size_t inputDims[] = {2, 6};

    tensor_t *inputTensor = initTensor(input, 2, inputDims);

    float conv1dForwardComplexOutput[6] = {
        -12.f, -10.f, // outputChannel 0
        11.f, 3.f, // outputChannel 1
        -6.f, 5.f // outputChannel 2
    };
    size_t conv1dOutputDims[] = {3, 2};
    tensor_t *conv1dOutputTensor = initTensor(conv1dForwardComplexOutput, 2, conv1dOutputDims);

    float delta[6] = {
        -1.f, 0.f, // oc0
        -2.f, -1.f, // oc1
        1.f, 0.f // oc2
    };
    size_t deltaDims[] = {2, 3};
    tensor_t *deltaTensor = initTensor(delta, 2, deltaDims);

    float target[6];
    for (size_t i = 0; i < 6; i++) {
        target[i] = conv1dOutputTensor->data[i] + deltaTensor->data[i];
    }
    tensor_t *targetTensor = initTensor(target, conv1dOutputTensor->numberOfDimensions, conv1dOutputTensor->dimensions);

    tensor_t *gradOut = MSELossBackward(conv1dOutputTensor, targetTensor);

    float expectedPropagatedLoss1[12] = {
        0.f, 0.1667f, 0.f, 0.5000f, 0.f, 0.f,
        0.f, 0.1667f, 0.1667f, -0.1667f, -0.1667f, 0.f
    };

    float expectedWeightGrad1[18] = {
        0.f, 0.3333f, 0.6667f, 0.f, -0.3333f, -0.6667f, // oc0
        0.5f, 1.5f, 1.3333f, -0.5f, -1.5f, -1.3333f, // oc1
        0.f, -0.3333f, -0.6667f, 0.f, 0.3333f, 0.6667f // oc2
    };

    float expectedBiasGrad1[3] = {0.1667f, 0.5f, -0.1667f};

    tensor_t *propagatedLoss1 = conv1dBackward(config, gradOut, &inputTensor);

    for (size_t i = 0; i < 12; ++i) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedPropagatedLoss1[i], propagatedLoss1->data[i]);
    }
    for (size_t i = 0; i < config->weight->size; ++i) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedWeightGrad1[i], config->weight->grad[i]);
    }
    for (size_t i = 0; i < config->bias->size; ++i) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedBiasGrad1[i], config->bias->grad[i]);
    }

    tensor_t *propagatedLoss2 = conv1dBackward(config, gradOut, inputTensor);
    for (size_t i = 0; i < 12; ++i) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedPropagatedLoss1[i], propagatedLoss2->data[i]);
    }
    for (size_t i = 0; i < config->weight->size; ++i) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedWeightGrad1[i] * 2.f, config->weight->grad[i]);
    }
    for (size_t i = 0; i < config->bias->size; ++i) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expectedBiasGrad1[i] * 2.f, config->bias->grad[i]);
    }
}


int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitConv1Config);
    RUN_TEST(unitTestCalcOutputSizeForInputSizeAndConv1dConfigSimple);
    RUN_TEST(unitTestCalcOutputSizeForInputSizeAndConv1dConfigComplex);

    RUN_TEST(unitTestConv1dForward);
    RUN_TEST(unitTestConv1dForwardComplex);

    RUN_TEST(unitTestConv1dBackward);
    RUN_TEST(unitTestConv1dBackwardComplex);
    return UNITY_END();
}
