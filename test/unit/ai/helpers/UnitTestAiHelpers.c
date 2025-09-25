#define SOURCE_FILE "AI-HELPERS-UTEST"

#include <stdlib.h>

#include "AiHelpers.h"
#include "unity.h"
#include "Linear.h"
#include "CSVReader.h"
#include "MSE.h"

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
    layerForward_t **model = calloc(1, sizeof(layerForward_t *));
    size_t modelSize = 1;
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
    float *input = csvParseRowAsFloat(csvRow, 3);

    float *actual = sequentialForward(model, modelSize, input);
    float expected_result[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, actual, linearConfig->outputSize);
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
        model, modelSize, MSELossDOutput, input, label);

    float expected_propagated_loss[] = {-3.f, -7.f, 9.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagatedLoss->output,
                                  sizeof(expected_propagated_loss) / sizeof(float));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitParameter);
    RUN_TEST(testSequentialForwardWithLinearAndCSV);
    RUN_TEST(testSequentialBackwardUsingLinearAndCSV);
    return UNITY_END();
}
