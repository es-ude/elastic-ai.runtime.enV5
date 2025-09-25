#define SOURCE_FILE "AI-HELPERS-UTEST"

#include <stdlib.h>

#include "AiHelpers.h"
#include "unity.h"
#include "Linear.h"
#include "CSVReader.h"
#include "MSE.h"
#include "CrossEntropy.h"
#include <Conv1d.h>
#include <string.h>

void setUp() {}

void tearDown() {}

void unitTestInitParameter() {
    float p[] = {1.f, 2.f, 3.f};
    size_t size = sizeof(p) / sizeof(p[0]);

    parameter_t *parameter = initParameter(p, size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(p, parameter->p, size);

    float expectedGrad[] = {0.f, 0.f, 0.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedGrad, parameter->grad, size);

    TEST_ASSERT_EQUAL_size_t(size, parameter->size);
}

void testSequentialForwardWithLinearAndCSV() {
    size_t modelSize = 1;

    layerForward_t **model = calloc(modelSize, sizeof(layerForward_t *));
    model[0] = malloc(sizeof(layerForward_t));

    size_t weightSize = 6;
    size_t biasSize = 2;
    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float bias[] = {-1.f, 3.f};

    layerForward_t *forward = initLinearLayerForwardWithWeightBias(
        weight, weightSize, bias, biasSize);
    linearConfig_t *linearConfig = forward->config;
    model[0] = forward;

    char *csvRow = csvReadRow(CSV_INPUT_PATH);
    float *input = csvParseRowAsFloat(csvRow, linearConfig->inputSize);

    float *actual = sequentialForward(model, modelSize, input, linearConfig->inputSize);
    float expected_result[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, actual, linearConfig->outputSize);
}

void testSequentialForwardWithConv1dAndCSV() {
    size_t inputSize = 12;
    size_t modelSize = 1;

    layerForward_t **model = calloc(modelSize, sizeof(layerForward_t *));

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 3, dilation = 2,
           paddingSize = 1;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f,
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f,
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f};
    float bias[] = {0.f, 1.f, 2.f};

    layerForward_t *forward = initConv1dLayerForward(weight, bias, inputChannels, outputChannels,
                                                     kernelSize, stride, dilation, ZEROS,
                                                     paddingSize, inputSize);

    conv1dConfig_t *conv1dConfig = forward->config;
    model[0] = forward;

    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloats = csvParseRowAsFloat(csvInputString, inputSize);

    float *actual = sequentialForward(model, modelSize, csvInputFloats, inputSize);
    float expected[12] = {
        -8.f, -8.f,
        8.f, 3.f,
        -4.f, 4.f
    };

    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(inputSize, conv1dConfig);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, outputSize);
}

void testSequentialBackwardUsingLinearAndCSV() {
    size_t modelSize = 1;
    layerForwardBackward_t **model = calloc(modelSize, sizeof(layerForwardBackward_t *));
    model[0] = malloc(sizeof(layerForwardBackward_t));

    size_t weightSize = 6;
    size_t biasSize = 2;
    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float bias[] = {-1.f, 3.f};

    layerForwardBackward_t *linearLayer = initLinearLayerForwardBackwardWithWeightBias(
        weight, weightSize, bias, biasSize);
    linearConfig_t *linearConfig = linearLayer->config;
    model[0] = linearLayer;

    size_t inputSize = linearConfig->inputSize;
    size_t labelSize = linearConfig->outputSize;
    float *input = calloc(inputSize, sizeof(float));
    float *label = calloc(labelSize, sizeof(float));

    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloats = csvParseRowAsFloat(csvInputString, linearConfig->inputSize);
    memcpy(input, csvInputFloats, inputSize * sizeof(float));

    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloats = csvParseRowAsFloat(csvLabelString, linearConfig->outputSize);
    memcpy(label, csvLabelFloats, labelSize * sizeof(float));

    trainingStats_t *propagatedLoss = sequentialCalculateGrads(
        model, modelSize, MSELossBackward, input, linearConfig->inputSize, label);

    float expected_propagated_loss[] = {-3.f, -7.f, 9.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagatedLoss->output,
                                  sizeof(expected_propagated_loss) / sizeof(float));
}

void testSequentialBackwardWithConv1dAndCSV() {
    size_t inputSize = 12;
    size_t modelSize = 1;

    layerForwardBackward_t **model = calloc(modelSize, sizeof(layerForwardBackward_t *));

    model[0] = malloc(sizeof(layerForwardBackward_t));

    // TODO struct für conv1d input parameter?

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 3, dilation = 2,
           paddingSize = 1;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f,
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f,
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f};
    float bias[] = {0.f, 1.f, 2.f};

    layerForwardBackward_t *conv1dLayer = initConv1dLayerForwardBackward(
        weight, bias, inputChannels, outputChannels, kernelSize, stride, dilation, ZEROS,
        paddingSize, inputSize);

    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(inputSize, conv1dLayer->config);

    model[0] = conv1dLayer;

    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloats = csvParseRowAsFloat(csvInputString, inputSize);

    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloats = csvParseRowAsFloat(csvLabelString, outputSize);

    trainingStats_t *actual = sequentialCalculateGrads(model, modelSize, MSELossBackward,
                                                       csvInputFloats, inputSize, csvLabelFloats);
    float expected[] = {0.0000f, 1.3333f, -0.3333f, 1.3333f, -1.0000f, 0.0000f,
                        0.0000f, -4.6667f, 0.0000f, -10.6667f, -5.6667f, 0.0000f};

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual->output[i]);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitParameter);

    RUN_TEST(testSequentialForwardWithLinearAndCSV);
    RUN_TEST(testSequentialBackwardUsingLinearAndCSV);

    RUN_TEST(testSequentialForwardWithConv1dAndCSV);
    RUN_TEST(testSequentialBackwardWithConv1dAndCSV);
    return UNITY_END();
}
