#define SOURCE_FILE "AI-HELPERS-UTEST"

#include <stdlib.h>

#include "AiHelpers.h"
#include "unity.h"
#include "Linear.h"
#include "CSVReader.h"
#include "Quantization.h"

#include "Conv1d.h"
#include "ReLU.h"
#include "Softmax.h"
#include <string.h>

void setUp() {}

void tearDown() {}

void unitTestInitParameterQTensor() {
    quantization_t *quantization = initQuantizationByType(FIXEDPOINT);

    float p[] = {1.f, 2.f, 3.f};
    size_t numberOfDimensions = 1;
    size_t dimensions[] = {3};

    parameterQTensor_t *parameterQTensor = initParameterQTensor(p, numberOfDimensions, dimensions, quantization);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(p, parameterQTensor->dataTensor->data, 3);

    float expectedGrad[] = {0.f, 0.f, 0.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedGrad, parameterQTensor->gradTensor->data, 3);

    TEST_ASSERT_EQUAL_size_t(3, calcTotalNumberOfElementsByTensor(parameterQTensor->dataTensor));
    TEST_ASSERT_EQUAL_size_t(3, calcTotalNumberOfElementsByTensor(parameterQTensor->gradTensor));

}

/*void testSequentialForwardWithLinearAndCSV() {
    size_t modelSize = 1;
    layerForward_t **model = calloc(modelSize, sizeof(layerForward_t *));

    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    size_t weightNumberOfDims = 2;
    size_t weightDims[] = {2, 3};
    parameterTensor_t *weightTensor = initParameterQTensor(weight, weightNumberOfDims, weightDims);

    float bias[] = {-1.f, 3.f};
    size_t biasNumberOfDims = 1;
    size_t biasDims[] = {2};
    parameterTensor_t *biasTensor = initParameterQTensor(bias, biasNumberOfDims, biasDims);

    size_t weightSize = calcTotalNumberOfElementsByTensor(weightTensor->tensor);
    size_t biasSize = calcTotalNumberOfElementsByTensor(biasTensor->tensor);

    layerForward_t *forward = initLinearLayerForwardWithWeightBias(
        weightTensor, biasTensor);
    model[0] = forward;

    char *csvRow = csvReadRow(CSV_INPUT_PATH);
    float *input = csvParseRowAsFloat(csvRow, weightSize / biasSize);
    size_t inputNumberOfDims = 2;
    size_t inputDims[] = {2, 3};
    tensor_t *inputTensor = initQTensor(input, inputNumberOfDims, inputDims);

    tensor_t *actual = sequentialForward(model, modelSize, inputTensor);
    float expected_result[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, actual->data, 2);
}

void testSequentialForwardWithConv1dAndCSV() {
    size_t modelSize = 1;
    layerForward_t **model = calloc(modelSize, sizeof(layerForward_t *));

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 3, dilation = 2,
           paddingSize = 1;

    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f,
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f,
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f};
    size_t weightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t weightNumberOfDims = sizeof(weightDims) / sizeof(weightDims[0]);
    parameterTensor_t *weightTensor = initParameterQTensor(weight, weightNumberOfDims, weightDims);

    float bias[] = {0.f, 1.f, 2.f};
    size_t biasNumberOfDims = 1;
    size_t biasDims[] = {3};
    parameterTensor_t *biasTensor = initParameterQTensor(bias, biasNumberOfDims, biasDims);

    layerForward_t *forward = initConv1dLayerForward(weightTensor, biasTensor, inputChannels,
                                                     outputChannels,
                                                     kernelSize, stride, dilation, ZEROS,
                                                     paddingSize);

    model[0] = forward;

    size_t inputNumberOfDims = 2;
    size_t inputDims[] = {2, 6};
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloats = csvParseRowAsFloat(csvInputString,
                                               calcTensorDataSize(inputNumberOfDims, inputDims));
    tensor_t *inputTensor = initQTensor(csvInputFloats, inputNumberOfDims, inputDims);

    tensor_t *actual = sequentialForward(model, modelSize, inputTensor);
    float expected[] = {
        -8.f, -8.f,
        8.f, 3.f,
        -4.f, 4.f
    };

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual->data, 6);
}

void testSequentialBackwardUsingLinearAndCSV() {
    size_t modelSize = 1;
    layerForwardBackward_t **model = calloc(modelSize, sizeof(layerForwardBackward_t *));

    float weight[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    size_t weightNumberOfDims = 2;
    size_t weightDims[] = {2, 3};
    parameterTensor_t *weightTensor = initParameterQTensor(weight, weightNumberOfDims, weightDims);

    float bias[] = {-1.f, 3.f};
    size_t biasNumberOfDims = 1;
    size_t biasDims[] = {2};
    parameterTensor_t *biasTensor = initParameterQTensor(bias, biasNumberOfDims, biasDims);

    layerForwardBackward_t *linearLayer = initLinearLayerForwardBackwardWithWeightBias(
        weightTensor, biasTensor);
    linearConfig_t *linearConfig = linearLayer->config;
    model[0] = linearLayer;

    size_t inputNumberOfDims = 1;
    size_t inputDims[] = {3};
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);

    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloats = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloats, inputNumberOfDims, inputDims);

    size_t labelNumberOfDims = 1;
    size_t labelDims[] = {2};
    size_t totalLabelSize = calcTensorDataSize(labelNumberOfDims, labelDims);

    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloats = csvParseRowAsFloat(csvLabelString, totalLabelSize);
    tensor_t *labelTensor = initQTensor(csvLabelFloats, labelNumberOfDims, labelDims);

    trainingStats_t *propagatedLoss = sequentialCalculateGrads(
        model, modelSize, MSE, inputTensor, labelTensor);

    float expected_propagated_loss[] = {-3.f, -7.f, 9.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagatedLoss->output,
                                  sizeof(expected_propagated_loss) / sizeof(float));
}

void testSequentialBackwardWithConv1dAndCSV() {
    size_t modelSize = 1;
    layerForwardBackward_t **model = calloc(modelSize, sizeof(layerForwardBackward_t *));

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, stride = 3, dilation = 2,
           paddingSize = 1;
    float weight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f,
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f,
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f};

    size_t weightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t weightNumberOfDims = sizeof(weightDims) / sizeof(weightDims[0]);

    parameterTensor_t *weightTensor = initParameterQTensor(weight, weightNumberOfDims, weightDims);

    float bias[] = {0.f, 1.f, 2.f};
    size_t biasNumberOfDims = 1;
    size_t biasDims[] = {3};
    parameterTensor_t *biasTensor = initParameterQTensor(bias, biasNumberOfDims, biasDims);

    layerForwardBackward_t *conv1dLayer = initConv1dLayerForwardBackward(
        weightTensor, biasTensor, inputChannels, outputChannels, kernelSize, stride, dilation,
        ZEROS,
        paddingSize);

    model[0] = conv1dLayer;

    size_t inputDims[] = {2, 6};
    size_t inputNumberOfDims = sizeof(inputDims) / sizeof(inputDims[0]);
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloats = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloats, inputNumberOfDims, inputDims);

    size_t labelNumberOfDims = 2;
    size_t labelDims[] = {3, 2};
    size_t totalLabelSize = calcTensorDataSize(labelNumberOfDims, labelDims);
    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloats = csvParseRowAsFloat(csvLabelString, totalLabelSize);
    tensor_t *labelTensor = initQTensor(csvLabelFloats, labelNumberOfDims, labelDims);

    trainingStats_t *actual = sequentialCalculateGrads(model, modelSize, MSE,
                                                       inputTensor, labelTensor);
    float expected[] = {0.0000f, 1.3333f, -0.3333f, 1.3333f, -1.0000f, 0.0000f,
                        0.0000f, -4.6667f, 0.0000f, -10.6667f, -5.6667f, 0.0000f};

    for (size_t i = 0; i < totalInputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual->output[i]);
    }
}

void test_Conv1d_ReLU_Linear_Forward() {
    size_t modelSize = 3;
    layerForward_t **model = malloc(modelSize * sizeof(layerForward_t));

    size_t inputDims[] = {1, 6};
    size_t inputNumberOfDims = sizeof(inputDims) / sizeof(size_t);
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloat = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloat, inputNumberOfDims, inputDims);

    size_t inputChannels = 1, outputChannels = 1, kernelSize = 3, dilation = 2, stride = 1,
           paddingSize = 1;

    float conv1dWeights[] = {-1.f, 0.f, 1.f};
    size_t conv1dWeightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t conv1dWeightNumberOfDims = sizeof(conv1dWeightDims) / sizeof(size_t);
    parameterTensor_t *conv1dWeightTensor = initParameterQTensor(
        conv1dWeights, conv1dWeightNumberOfDims, conv1dWeightDims);

    float conv1dBias[] = {0.f};
    size_t conv1dBiasDims[] = {1};
    size_t conv1dBiasNumberOfDims = sizeof(conv1dBiasDims) / sizeof(size_t);
    parameterTensor_t *conv1dBiasTensor = initParameterQTensor(
        conv1dBias, conv1dBiasNumberOfDims, conv1dBiasDims);

    layerForward_t *conv1dLayer = initConv1dLayerForward(
        conv1dWeightTensor, conv1dBiasTensor, inputChannels, outputChannels, kernelSize, stride,
        dilation,
        SAME, paddingSize);

    layerForward_t *reluLayer = initReLULayerForward();

    float linearWeights[] = {-1.f, 0.f, 1.f, -1.f, 2.f, 3.f};
    size_t linearWeightDims[] = {1, 6};
    size_t linearWeightNumberOfDims = sizeof(linearWeightDims) / sizeof(size_t);
    parameterTensor_t *linearWeightTensor = initParameterQTensor(
        linearWeights, linearWeightNumberOfDims, linearWeightDims);

    float linearBias[] = {-1.f};
    size_t linearBiasDims[] = {1};
    size_t linearBiasNumberOfDims = sizeof(linearBiasDims) / sizeof(size_t);
    parameterTensor_t *linearBiasTensor = initParameterQTensor(
        linearBias, linearBiasNumberOfDims, linearBiasDims);

    layerForward_t *linearLayer = initLinearLayerForwardWithWeightBias(
        linearWeightTensor, linearBiasTensor);

    model[0] = conv1dLayer;
    model[1] = reluLayer;
    model[2] = linearLayer;

    tensor_t *actual = sequentialForward(model, modelSize, inputTensor);

    TEST_ASSERT_EQUAL_FLOAT(-3, actual->data[0]);
}

void test_Conv1d_ReLU_Linear_MSE_Backward() {
    size_t modelSize = 3;
    layerForwardBackward_t **model = malloc(modelSize * sizeof(layerForwardBackward_t));

    size_t inputDims[] = {1, 6};
    size_t inputNumberOfDims = sizeof(inputDims) / sizeof(size_t);
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloat = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloat, inputNumberOfDims, inputDims);

    size_t inputChannels = 1, outputChannels = 1, kernelSize = 3, dilation = 2, stride = 1,
           paddingSize = 1;

    float conv1dWeights[] = {-1.f, 0.f, 1.f};
    size_t conv1dWeightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t conv1dWeightNumberOfDims = sizeof(conv1dWeightDims) / sizeof(size_t);
    parameterTensor_t *conv1dWeightTensor = initParameterQTensor(
        conv1dWeights, conv1dWeightNumberOfDims, conv1dWeightDims);

    float conv1dBias[] = {0.f};
    size_t conv1dBiasDims[] = {1};
    size_t conv1dBiasNumberOfDims = sizeof(conv1dBiasDims) / sizeof(size_t);
    parameterTensor_t *conv1dBiasTensor = initParameterQTensor(
        conv1dBias, conv1dBiasNumberOfDims, conv1dBiasDims);

    layerForwardBackward_t *conv1dLayer = initConv1dLayerForwardBackward(
        conv1dWeightTensor, conv1dBiasTensor, inputChannels, outputChannels, kernelSize, stride,
        dilation,
        SAME, paddingSize);

    layerForwardBackward_t *reluLayer = initReLULayerForwardBackward();

    float linearWeights[] = {-1.f, 0.f, 1.f, -1.f, 2.f, 3.f};
    size_t linearWeightDims[] = {1, 6};
    size_t linearWeightNumberOfDims = sizeof(linearWeightDims) / sizeof(size_t);
    parameterTensor_t *linearWeightTensor = initParameterQTensor(
        linearWeights, linearWeightNumberOfDims, linearWeightDims);

    float linearBias[] = {-1.f};
    size_t linearBiasDims[] = {1};
    size_t linearBiasNumberOfDims = sizeof(linearBiasDims) / sizeof(size_t);
    parameterTensor_t *linearBiasTensor = initParameterQTensor(
        linearBias, linearBiasNumberOfDims, linearBiasDims);

    layerForwardBackward_t *linearLayer = initLinearLayerForwardBackwardWithWeightBias(
        linearWeightTensor, linearBiasTensor);

    model[0] = conv1dLayer;
    model[1] = reluLayer;
    model[2] = linearLayer;

    size_t labelDims[] = {2};
    size_t labelNumberOfDims = sizeof(labelDims) / sizeof(size_t);
    size_t totalLabelSize = calcTensorDataSize(labelNumberOfDims, labelDims);
    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloat = csvParseRowAsFloat(csvLabelString, totalLabelSize);
    tensor_t *labelTensor = initQTensor(csvLabelFloat, labelNumberOfDims, labelDims);

    trainingStats_t *actual = sequentialCalculateGrads(model, modelSize, MSE,
                                                       inputTensor,
                                                       labelTensor);

    float expected[] = {-2.f, 2.f, -2.f, 0.f, 2.f, -2.f};

    for (size_t i = 0; i < totalInputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual->output[i]);
    }
}

void test_Conv1d_ReLU_Linear_MSE_Backward_Complex_No_Dilation() {
    size_t modelSize = 3;
    layerForwardBackward_t **model = malloc(modelSize * sizeof(layerForwardBackward_t));

    size_t inputDims[] = {2, 6};
    size_t inputNumberOfDims = sizeof(inputDims) / sizeof(size_t);
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloat = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloat, inputNumberOfDims, inputDims);

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, dilation = 1, stride = 1,
           paddingSize = 1;

    float conv1dWeights[] = {1.f, 0.f, -1.f, 0.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, 1.f, -1.f, 1.f,
                             0.f, -1.f, 1.f, 1.f, 0.f, -1.f};
    size_t conv1dWeightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t conv1dWeightNumberOfDims = sizeof(conv1dWeightDims) / sizeof(size_t);
    parameterTensor_t *conv1dWeightTensor = initParameterQTensor(
        conv1dWeights, conv1dWeightNumberOfDims, conv1dWeightDims);

    float conv1dBias[] = {0.f, 1.f, -1.f};
    size_t conv1dBiasDims[] = {3};
    size_t conv1dBiasNumberOfDims = sizeof(conv1dBiasDims) / sizeof(size_t);
    parameterTensor_t *conv1dBiasTensor = initParameterQTensor(
        conv1dBias, conv1dBiasNumberOfDims, conv1dBiasDims);

    layerForwardBackward_t *conv1dLayer = initConv1dLayerForwardBackward(
        conv1dWeightTensor, conv1dBiasTensor, inputChannels, outputChannels, kernelSize, stride,
        dilation,
        SAME, paddingSize);

    layerForwardBackward_t *reluLayer = initReLULayerForwardBackward(
        initConv1dOutputTensor(conv1dLayer->config, inputTensor));

    float linearWeights[] = {1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                             1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                             1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                             -1.f, 1.f, 0.f, -1.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, -1.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, -1.f, 1.f, 0.f};
    size_t linearWeightDims[] = {2, 18};
    size_t linearWeightNumberOfDims = sizeof(linearWeightDims) / sizeof(size_t);
    parameterTensor_t *linearWeightTensor = initParameterQTensor(
        linearWeights, linearWeightNumberOfDims, linearWeightDims);

    float linearBias[] = {0.f, 1.f};
    size_t lineaBiasDims[] = {2};
    size_t linearBiasNumberOfDims = sizeof(lineaBiasDims) / sizeof(size_t);
    parameterTensor_t *linearBiasTensor = initParameterQTensor(
        linearBias, linearBiasNumberOfDims, lineaBiasDims);

    layerForwardBackward_t *linearLayer = initLinearLayerForwardBackwardWithWeightBias(
        linearWeightTensor, linearBiasTensor);

    model[0] = conv1dLayer;
    model[1] = reluLayer;
    model[2] = linearLayer;

    size_t labelDims[] = {2};
    size_t labelNumberOfDims = sizeof(labelDims) / sizeof(size_t);
    size_t labelSize = calcTensorDataSize(labelNumberOfDims, labelDims);
    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloat = csvParseRowAsFloat(csvLabelString, labelSize);
    tensor_t *labelTensor = initQTensor(csvLabelFloat, labelNumberOfDims, labelDims);

    trainingStats_t *actual = sequentialCalculateGrads(model, modelSize, MSE, inputTensor,
                                                       labelTensor);

    float expected[] = {-2.f, -4.f, 8.f, 2.f, -8.f, 4.f,
                        12.f, -4.f, -4.f, 8.f, 2.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual->output, totalInputSize);
}


void test_Conv1d_ReLU_Linear_MSE_Backward_Complex_With_Dilation() {
    size_t modelSize = 3;
    layerForwardBackward_t **model = malloc(modelSize * sizeof(layerForwardBackward_t));

    size_t inputDims[] = {2, 6};
    size_t inputNumberOfDims = sizeof(inputDims) / sizeof(size_t);
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloat = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloat, inputNumberOfDims, inputDims);

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, dilation = 2, stride = 1,
           paddingSize = 1;

    float conv1dWeights[] = {1.f, 0.f, -1.f, 0.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, 1.f, -1.f, 1.f,
                             0.f, -1.f, 1.f, 1.f, 0.f, -1.f};
    size_t conv1dWeightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t conv1dWeightNumberOfDims = sizeof(conv1dWeightDims) / sizeof(size_t);
    parameterTensor_t *conv1dWeightTensor = initParameterQTensor(
        conv1dWeights, conv1dWeightNumberOfDims, conv1dWeightDims);

    float conv1dBias[] = {0.f, 1.f, -1.f};
    size_t conv1dBiasDims[] = {3};
    size_t conv1dBiasNumberOfDims = sizeof(conv1dBiasDims) / sizeof(size_t);
    parameterTensor_t *conv1dBiasTensor = initParameterQTensor(
        conv1dBias, conv1dBiasNumberOfDims, conv1dBiasDims);

    layerForwardBackward_t *conv1dLayer = initConv1dLayerForwardBackward(
        conv1dWeightTensor, conv1dBiasTensor, inputChannels, outputChannels, kernelSize, stride,
        dilation,
        SAME, paddingSize);

    layerForwardBackward_t *reluLayer = initReLULayerForwardBackward(
        initConv1dOutputTensor(conv1dLayer->config, inputTensor));

    float linearWeights[] = {1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                             1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                             1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                             -1.f, 1.f, 0.f, -1.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, -1.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, -1.f, 1.f, 0.f};
    size_t linearWeightDims[] = {2, 18};
    size_t linearWeightNumberOfDims = sizeof(linearWeightDims) / sizeof(size_t);
    parameterTensor_t *linearWeightTensor = initParameterQTensor(
        linearWeights, linearWeightNumberOfDims, linearWeightDims);

    float linearBias[] = {0.f, 1.f};
    size_t lineaBiasDims[] = {2};
    size_t linearBiasNumberOfDims = sizeof(lineaBiasDims) / sizeof(size_t);
    parameterTensor_t *linearBiasTensor = initParameterQTensor(
        linearBias, linearBiasNumberOfDims, lineaBiasDims);

    layerForwardBackward_t *linearLayer = initLinearLayerForwardBackwardWithWeightBias(
        linearWeightTensor, linearBiasTensor);

    model[0] = conv1dLayer;
    model[1] = reluLayer;
    model[2] = linearLayer;

    size_t labelDims[] = {2};
    size_t labelNumberOfDims = sizeof(labelDims) / sizeof(size_t);
    size_t labelSize = calcTensorDataSize(labelNumberOfDims, labelDims);
    char *csvLabelString = csvReadRow(CSV_LABEL_PATH);
    float *csvLabelFloat = csvParseRowAsFloat(csvLabelString, labelSize);
    tensor_t *labelTensor = initQTensor(csvLabelFloat, labelNumberOfDims, labelDims);

    trainingStats_t *actual = sequentialCalculateGrads(model, modelSize, MSE, inputTensor,
                                                       labelTensor);

    float expected[] = {5.f, -5.f, -9.f, 10.f, 4.f, -5.f,
                        -2.f, -4.f, 10.f, -6.f, -5.f, 5.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual->output, totalInputSize);
}


void test_Conv1d_ReLU_Linear_Softmax_CrossEntropy() {
    size_t modelSize = 4;
    layerForwardBackward_t **model = malloc(modelSize * sizeof(layerForwardBackward_t));

    size_t inputDims[] = {2, 6};
    size_t inputNumberOfDims = 2;
    size_t totalInputSize = calcTensorDataSize(inputNumberOfDims, inputDims);
    char *csvInputString = csvReadRow(CSV_INPUT_PATH);
    float *csvInputFloat = csvParseRowAsFloat(csvInputString, totalInputSize);
    tensor_t *inputTensor = initQTensor(csvInputFloat, inputNumberOfDims, inputDims);

    size_t inputChannels = 2, outputChannels = 3, kernelSize = 3, dilation = 2, stride = 1,
           paddingSize = 1;

    float conv1dWeight[] = {
        -1.f, 0.f, 1.f, -1.f, 2.f, 3.f,
        0.f, 0.f, 0.f, 1.f, -1.f, -2.f,
        -2.f, -1.f, -2.f, -3.f, -1.f, 0.f};
    size_t conv1dWeightDims[] = {outputChannels, inputChannels, kernelSize};
    size_t conv1dWeightNumberOfDims = sizeof(conv1dWeightDims) / sizeof(size_t);
    parameterTensor_t *conv1dWeightTensor = initParameterQTensor(
        conv1dWeight, conv1dWeightNumberOfDims, conv1dWeightDims);

    float conv1dBias[] = {0.f, 1.f, 2.f};
    size_t conv1dBiasDims[] = {3};
    size_t conv1dBiasNumberOfDims = sizeof(conv1dBiasDims) / sizeof(size_t);
    parameterTensor_t *conv1dBiasTensor = initParameterQTensor(
        conv1dBias, conv1dBiasNumberOfDims, conv1dBiasDims);

    layerForwardBackward_t *conv1dLayer = initConv1dLayerForwardBackward(
        conv1dWeightTensor, conv1dBiasTensor, inputChannels, outputChannels, kernelSize, stride,
        dilation,
        SAME,
        paddingSize);

    layerForwardBackward_t *reluLayer = initReLULayerForwardBackward();

    float linearWeight[] = {1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                            1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                            1.f, 0.f, -1.f, 1.f, 0.f, -1.f,
                            -1.f, 1.f, 0.f, -1.f, 1.f, 0.f,
                            -1.f, 1.f, 0.f, -1.f, 1.f, 0.f,
                            -1.f, 1.f, 0.f, -1.f, 1.f, 0.f};
    size_t linearWeightDims[] = {2, 18};
    size_t linearWeightNumberOfDims = sizeof(linearWeightDims) / sizeof(size_t);
    parameterTensor_t *linearWeightTensor = initParameterQTensor(
        linearWeight, linearWeightNumberOfDims, linearWeightDims);



    float linearBias[] = {0.f, 1.f};
    size_t linearBiasDims[] = {2};
    size_t linearBiasNumberOfDims = sizeof(linearBiasDims) / sizeof(size_t);
    parameterTensor_t *linearBiasTensor = initParameterQTensor(
        linearBias, linearBiasNumberOfDims, linearBiasDims);

    layerForwardBackward_t *linearLayer = initLinearLayerForwardBackwardWithWeightBias(
        linearWeightTensor, linearBiasTensor);

    layerForwardBackward_t *softmaxLayer = initSoftmaxLayerForwardBackward();

    model[0] = conv1dLayer;
    model[1] = reluLayer;
    model[2] = linearLayer;
    model[3] = softmaxLayer;

    size_t distDims[] = {2};
    size_t distNumberOfDims = sizeof(distDims) / sizeof(size_t);
    size_t totalDistSize = calcTensorDataSize(distNumberOfDims, distDims);
    char *csvDistributionString = csvReadRow(CSV_DIST_PATH);
    float *csvDistributionFloat = csvParseRowAsFloat(csvDistributionString, totalDistSize);
    tensor_t *distTensor = initQTensor(csvDistributionFloat, distNumberOfDims, distDims);



    trainingStats_t *actual = sequentialCalculateGrads(model, modelSize,
                                                       CROSS_ENTROPY,
                                                       inputTensor, distTensor);

    float expected[] = {0.0000f, 0.0000f, 1.2621f, 1.2621f, 0.6311f, 0.6311f,
                        -1.8932f, 1.8932f, -0.6311f, 1.2621f, 2.5242f, -1.2621f};

    for (size_t i = 0; i < totalInputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual->output[i]);
    }

}*/

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitParameterQTensor);

    RUN_TEST(testSequentialForwardWithLinearAndCSV);
    RUN_TEST(testSequentialBackwardUsingLinearAndCSV);

    RUN_TEST(testSequentialForwardWithConv1dAndCSV);
    RUN_TEST(testSequentialBackwardWithConv1dAndCSV);

    RUN_TEST(test_Conv1d_ReLU_Linear_Forward);
    RUN_TEST(test_Conv1d_ReLU_Linear_MSE_Backward);

    RUN_TEST(test_Conv1d_ReLU_Linear_MSE_Backward_Complex_No_Dilation);
    RUN_TEST(test_Conv1d_ReLU_Linear_MSE_Backward_Complex_With_Dilation);

    RUN_TEST(test_Conv1d_ReLU_Linear_Softmax_CrossEntropy);
    return UNITY_END();
}
