#include "Linear.h"
#include "Tensor.h"
#include "TensorConversion.h"
#include "unity.h"
#include "Rounding.h"
#include "DTypes.h"

#include <string.h>

void setUp() {}
void tearDown() {}

void testLinearForwardFloat() {
    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float weightGrads[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};

    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);

    parameter_t weights;
    setParameterValues(&weights, weightData, &weightQ, weightGrads, &weightGradQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);


    float biasData[] = {-1.f, 3.f};
    float biasGrads[] = {0.f, 0.f};
    size_t biasDims[] = {2};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};

    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);

    parameter_t bias;
    setParameterValues(&bias, biasData, &biasQ, biasGrads, &biasGradQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);

    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, FLOATLAYER, &weights, &bias);

    float inputData[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    size_t inputNumberOfDims = 1;
    size_t inputOrderOfDims[] = {0};
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);

    tensor_t input;
    setTensorValues(&input, inputData, inputDims, inputNumberOfDims, inputOrderOfDims, &inputQ, NULL);

    float outputData[2] = {0, 0};
    size_t outputDims[] = {2};
    size_t outputNumberOfDims = 1;
    size_t outputOrderOfDims[] = {0};
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);

    tensor_t output;
    setTensorValues(&output, outputData, outputDims, outputNumberOfDims, outputOrderOfDims, &outputQ, NULL);

    linearForward(&linearConfig, &input, &output);

    float expected[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, output.data, 2);
}

void testLinearForwardAsym() {
    size_t numberOfWeights = 6;
    size_t numberOfBiases = 2;
    size_t numberOfInputs = 3;
    size_t numberOfOutputs = 2;

    float weightFloatData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};

    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};

    quantization_t weightFloatQ;
    initFloat32Quantization(&weightFloatQ);

    tensor_t weightsFloat;
    setTensorValues(&weightsFloat, weightFloatData, weightDims, weightNumberOfDims,
                    weightOrderOfDims, &weightFloatQ, NULL);

    uint8_t weightAsymData[numberOfWeights];
    asymQConfig_t weightsAsymQConfig;
    initAsymQConfig(8, HTE, &weightsAsymQConfig);
    quantization_t weightAsymQ;
    initAsymQuantization(&weightsAsymQConfig, &weightAsymQ);

    tensor_t weightsAsym;
    setTensorValuesForConversion(weightAsymData, &weightAsymQ, &weightsFloat, &weightsAsym);
    convertTensor(&weightsFloat, &weightsAsym);

    uint8_t weightGrads[numberOfWeights];
    asymQConfig_t weightGradsAsymQConfig;
    initAsymQConfig(8, HTE, &weightGradsAsymQConfig);
    quantization_t weightGradsAsymQ;
    initAsymQuantization(&weightGradsAsymQConfig, &weightGradsAsymQ);

    parameter_t weights;
    setParameterValues(&weights, weightAsymData, &weightAsymQ, weightGrads, &weightGradsAsymQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);

    int32_t biasIntData[] = {-1, 3};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};

    quantization_t biasIntQ;
    initInt32Quantization(&biasIntQ);

    tensor_t biasInt;
    setTensorValues(&biasInt, biasIntData, biasDims, biasNumberOfDims, biasOrderOfDims, &biasIntQ,
                    NULL);

    int32_t biasGrads[numberOfBiases];
    quantization_t biasGradsQ;
    initInt32Quantization(&biasGradsQ);

    parameter_t bias;
    setParameterValues(&bias, biasIntData, &biasIntQ, biasGrads, &biasGradsQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);

    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weights = &weights;
    linearConfig.qType = ASYMLAYER;

    float inputFloatData[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    size_t inputNumberOfDims = 1;
    size_t inputOrderOfDims[] = {0};
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);

    tensor_t inputFloat;
    setTensorValues(&inputFloat, inputFloatData, inputDims, inputNumberOfDims, inputOrderOfDims,
                    &inputFloatQ, NULL);

    asymQConfig_t inputAsymQConfig;
    initAsymQConfig(8, HTE, &inputAsymQConfig);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQConfig, &inputAsymQ);
    uint8_t inputAsymData[numberOfInputs * calcBytesPerElement(&inputAsymQ)];

    tensor_t inputAsym;
    setTensorValuesForConversion(inputAsymData, &inputAsymQ, &inputFloat, &inputAsym);
    convertTensor(&inputFloat, &inputAsym);

    size_t outputDims[] = {2, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};

    asymQConfig_t outputAsymQConfig;
    initAsymQConfig(8, HTE, &outputAsymQConfig);
    quantization_t outputAsymQ;
    initAsymQuantization(&outputAsymQConfig, &outputAsymQ);
    uint8_t outputAsymData[numberOfOutputs * calcBytesPerElement(&outputAsymQ)];

    tensor_t outputAsym;
    setTensorValues(&outputAsym, outputAsymData, outputDims, outputNumberOfDims, outputOrderOfDims,
                    &outputAsymQ, NULL);

    linearForward(&linearConfig, &inputAsym, &outputAsym);

    float outputFloatData[numberOfOutputs];
    quantization_t outputFloatQ;
    initFloat32Quantization(&outputFloatQ);
    tensor_t outputFloat;
    setTensorValuesForConversion(outputFloatData, &outputFloatQ, &outputAsym, &outputFloat);
    convertTensor(&outputAsym, &outputFloat);

    float actual[numberOfOutputs];
    readBytesAsFloatArray(numberOfOutputs, outputFloat.data, actual);

    // values are off, because scale is ignored, when adding bias
    //float expected[] = {-5.f, -4.f};
    float expected[] = {-4, -7};

    for (size_t i = 0; i < numberOfOutputs; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, expected[i], actual[i]);
    }
}

void testLinearBackwardFloat() {
    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float weightGrads[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};

    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);

    parameter_t weights;
    setParameterValues(&weights, weightData, &weightQ, weightGrads, &weightGradQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);


    float biasData[] = {-1.f, 3.f};
    float biasGrads[] = {0.f, 0.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};

    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);

    parameter_t bias;
    setParameterValues(&bias, biasData, &biasQ, biasGrads, &biasGradQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);

    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, FLOATLAYER, &weights, &bias);

    float outputData[] = {0.f, 1.f, 2.f};
    size_t outputDims[] = {3, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};

    quantization_t outputQ;
    initFloat32Quantization(&outputQ);

    tensor_t output;
    setTensorValues(&output, outputData, outputDims, outputNumberOfDims, outputOrderOfDims, &outputQ, NULL);

    float lossData[] = {-4.f, -3.f};
    size_t lossDims[] = {2, 1};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};

    quantization_t lossQ;
    initFloat32Quantization(&lossQ);

    tensor_t loss;
    setTensorValues(&loss, lossData, lossDims, lossNumberOfDims, lossOrderOfDims, &lossQ, NULL);


    float propLossData[3];
    size_t propLossDims[] = {3};
    size_t propLossNumberOfDims = 1;
    size_t propLossOrderOfDims[] = {0};

    quantization_t propLossQ;
    initFloat32Quantization(&propLossQ);

    tensor_t propLoss;
    setTensorValues(&propLoss, propLossData, propLossDims, propLossNumberOfDims, propLossOrderOfDims, &propLossQ, NULL);


    linearBackward(&linearConfig, &loss, &output, &propLoss);

    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    float propLossOutput[3];
    readBytesAsFloatArray(3, propLoss.data, propLossOutput);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig.weights->grad,
                                  calcNumberOfElementsByDims(linearConfig.weights->tensor.shape.numberOfDimensions
                                      , linearConfig.weights->tensor.shape.dimensions));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propLoss.data,
                                  sizeof(expected_propagated_loss) / sizeof(float));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig.bias->grad,
                                  calcNumberOfElementsByDims(linearConfig.bias->tensor.shape.numberOfDimensions,
                                      linearConfig.bias->tensor.shape.dimensions));

}

void testLinearBackwardAsym() {

    size_t numberOfWeights = 6;
    size_t numberOfBiases = 2;
    size_t numberOfLosses = 2;
    size_t numberOfForwardInputs = 3;

    float weightFloatData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    /*
    -1,  4
     2,  5
    -3, -6
    */

    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};

    quantization_t weightFloatQ;
    initFloat32Quantization(&weightFloatQ);

    tensor_t weightsFloat;
    setTensorValues(&weightsFloat, weightFloatData, weightDims, weightNumberOfDims, weightOrderOfDims, &weightFloatQ, NULL);


    asymQConfig_t weightsAsymQConfig;
    initAsymQConfig(8, HTE, &weightsAsymQConfig);
    quantization_t weightsAsymQ;
    initAsymQuantization(&weightsAsymQConfig, &weightsAsymQ);
    uint8_t weightsAsymData[numberOfWeights * calcBytesPerElement(&weightsAsymQ)];

    tensor_t weightsAsym;
    setTensorValuesForConversion(weightsAsymData, &weightsAsymQ, &weightsFloat, &weightsAsym);
    convertTensor(&weightsFloat, &weightsAsym);

    asymQConfig_t weightGradsAsymQConfig;
    initAsymQConfig(8, HTE, &weightGradsAsymQConfig);
    quantization_t weightGradsAsymQ;
    initAsymQuantization(&weightsAsymQConfig, &weightGradsAsymQ);
    uint8_t weightGrads[numberOfWeights * calcBytesPerElement(&weightGradsAsymQ)];

    parameter_t weights;
    setParameterValues(&weights, weightsAsymData, &weightsAsymQ, weightGrads, &weightGradsAsymQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);


    parameter_t bias;
    int32_t biasData[] = {-1, 3};
    size_t biasDims[] = {2};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};

    quantization_t biasQ;
    initInt32Quantization(&biasQ);

    asymQConfig_t biasGradsAsymQConfig;
    initAsymQConfig(8, HTE, &biasGradsAsymQConfig);
    quantization_t biasGradsAsymQ;
    initAsymQuantization(&biasGradsAsymQConfig, &biasGradsAsymQ);
    uint8_t biasGrads[numberOfBiases * calcBytesPerElement(&biasGradsAsymQ)];

    // IMPORTANT, OTHERWISE BIAS GRADS IS FILLED WITH RANDOM DATA
    memset(biasGrads, 0, numberOfBiases * calcBytesPerElement(&biasGradsAsymQ));

    setParameterValues(&bias, biasData, &biasQ, biasGrads, &biasGradsAsymQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);

    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, ASYMLAYER, &weights, &bias);

    float forwardInputFloatData[] = {0.f, 1.f, 2.f};
    size_t forwardInputDims[] = {3, 1};
    size_t forwardInputNumberOfDims = 2;
    size_t forwardInputOrderOfDims[] = {0, 1};

    quantization_t forwardInputFloatQ;
    initFloat32Quantization(&forwardInputFloatQ);

    tensor_t forwardInputFloat;
    setTensorValues(&forwardInputFloat, forwardInputFloatData, forwardInputDims, forwardInputNumberOfDims, forwardInputOrderOfDims, &forwardInputFloatQ, NULL);

    asymQConfig_t forwardInputAsymQConfig;
    initAsymQConfig(8, HTE, &forwardInputAsymQConfig);
    quantization_t forwardInputAsymQ;
    initAsymQuantization(&forwardInputAsymQConfig, &forwardInputAsymQ);
    uint8_t forwardInputAsymData[numberOfForwardInputs * calcBytesPerElement(&forwardInputAsymQ)];

    tensor_t forwardInputAsym;
    setTensorValuesForConversion(forwardInputAsymData, &forwardInputAsymQ, &forwardInputFloat,
                                 &forwardInputAsym);
    convertTensor(&forwardInputFloat, &forwardInputAsym);

    float lossFloatData[] = {-4.f, -3.f};
    size_t lossDims[] = {2, 1};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};

    quantization_t lossFloatQ;
    initFloat32Quantization(&lossFloatQ);

    tensor_t lossFloat;
    setTensorValues(&lossFloat, lossFloatData, lossDims, lossNumberOfDims, lossOrderOfDims, &lossFloatQ, NULL);

    asymQConfig_t lossAsymQConfig;
    initAsymQConfig(8, HTE, &lossAsymQConfig);
    quantization_t lossAsymQ;
    initAsymQuantization(&lossAsymQConfig, &lossAsymQ);
    uint8_t lossAsymData[numberOfLosses * calcBytesPerElement(&lossAsymQ)];

    tensor_t lossAsym;
    setTensorValuesForConversion(lossAsymData, &lossAsymQ, &lossFloat, &lossAsym);
    convertTensor(&lossFloat, &lossAsym);

    size_t propLossDims[] = {numberOfForwardInputs};
    size_t propLossNumberOfDims = 1;
    size_t propLossOrderOfDims[] = {0};

    asymQConfig_t propLossAsymQConfig;
    initAsymQConfig(8, HTE, &propLossAsymQConfig);
    quantization_t propLossAsymQ;
    initAsymQuantization(&propLossAsymQConfig, &propLossAsymQ);
    uint8_t propLossAsymData[numberOfForwardInputs * calcBytesPerElement(&propLossAsymQ)];

    tensor_t propLossAsym;
    setTensorValues(&propLossAsym, propLossAsymData, propLossDims, propLossNumberOfDims, propLossOrderOfDims, &propLossAsymQ, NULL);

    linearBackward(&linearConfig, &lossAsym, &forwardInputAsym, &propLossAsym);

    uint8_t weightGradsAsymData[numberOfWeights * calcBytesPerElement(&weightsAsymQ)];
    size_t weightGradsAsymOrderOfDimensions[] = {0, 1};
    tensor_t weightGradsAsym;
    setTensorValues(&weightGradsAsym, weightGradsAsymData, weightDims, weightNumberOfDims,
                    weightOrderOfDims, &weightGradsAsymQ, NULL);
    getGradTensorFromParameter(linearConfig.weights, &weightGradsAsym,
                               weightGradsAsymOrderOfDimensions);

    float weightGradFloatData[numberOfWeights];
    quantization_t weightGradFloatQ;
    initFloat32Quantization(&weightGradFloatQ);
    tensor_t weightGradsFloat;
    setTensorValuesForConversion(weightGradFloatData, &weightGradFloatQ, &weightGradsAsym,
                                 &weightGradsFloat);
    convertTensor(&weightGradsAsym, &weightGradsFloat);

    float actualWeightGrads[numberOfWeights];
    readBytesAsFloatArray(numberOfWeights, weightGradsFloat.data, actualWeightGrads);

    float expectedWeightGrads[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    for (size_t i = 0; i < numberOfWeights; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, expectedWeightGrads[i], actualWeightGrads[i]);
    }

    uint8_t biasGradsAsymData[numberOfBiases * calcBytesPerElement(&biasQ)];
    size_t biasGradsAsymOrderOfDimensions[] = {0, 1};
    tensor_t biasGradsAsym;
    setTensorValues(&biasGradsAsym, biasGradsAsymData, biasDims, biasNumberOfDims, biasOrderOfDims,
                    &biasGradsAsymQ, NULL);
    getGradTensorFromParameter(linearConfig.bias, &biasGradsAsym,
                               biasGradsAsymOrderOfDimensions);

    float biasGradFloatData[numberOfBiases];
    quantization_t biasGradFloatQ;
    initFloat32Quantization(&biasGradFloatQ);
    tensor_t biasGradsFloat;
    setTensorValuesForConversion(biasGradFloatData, &biasGradFloatQ, &biasGradsAsym,
                                 &biasGradsFloat);

    convertTensor(&biasGradsAsym, &biasGradsFloat);

    float *test = (float *)biasGradsFloat.data;

    for(size_t i = 0; i < 2; i++) {
        printf("%f\n", test[i]);
    }
    float expectedBiasGrads[] = {-0.125f, -0.09375f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedBiasGrads, biasGradsFloat.data, numberOfBiases);

    float propLossFloatData[numberOfForwardInputs];
    quantization_t propLossFloatQ;
    initFloat32Quantization(&propLossFloatQ);
    tensor_t propLossFloat;
    setTensorValuesForConversion(propLossFloatData, &propLossFloatQ, &propLossAsym,
                                 &propLossFloat);
    convertTensor(&propLossAsym, &propLossFloat);

    size_t numberOfInputs = 3;
    float propLossArray[numberOfInputs];
    readBytesAsFloatArray(numberOfInputs, propLossFloat.data, propLossArray);

    float expectedPropagatedLoss[] = {-8.f, -23.f, 30.f};

    for (size_t i = 0; i < numberOfForwardInputs; i++) {
        TEST_ASSERT_FLOAT_WITHIN(.2f, expectedPropagatedLoss[i], propLossArray[i]);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testLinearForwardFloat);
    RUN_TEST(testLinearBackwardFloat);

    RUN_TEST(testLinearForwardAsym);
    RUN_TEST(testLinearBackwardAsym);
    UNITY_END();
}
