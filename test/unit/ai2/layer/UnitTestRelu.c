#include "Relu.h"
#include "Quantization.h"
#include "unity.h"
#include "DTypes.h"
#include "Tensor.h"
#include "TensorConversion.h"

void testReluForwardFloat() {
    size_t numberOfElements = 6;

    tensor_t input;
    float inputData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape = {
        .dimensions = inputDims,
        .orderOfDimensions = inputOrderOfDims,
        .numberOfDimensions = inputNumberOfDims
    };
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&input, inputData, &inputShape,
                    &inputQ, NULL);

    tensor_t output;
    float outputData[numberOfElements];
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    shape_t outputShape = {
        .dimensions = outputDims,
        .orderOfDimensions = outputOrderOfDims,
        .numberOfDimensions = outputNumberOfDims
    };
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    setTensorValues(&output, outputData, &outputShape, &outputQ, NULL);

    layer_t reluLayer;
    initLayer(&reluLayer, RELU, NULL, FLOAT_LAYER, &inputQ, &outputQ);

    reluForward(&reluLayer, &input, &output);

    float expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, output.data, actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, numberOfElements);
}

void testReluForwardAsym() {
    size_t numberOfElements = 6;

    tensor_t inputFloat;
    float inputFloatData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape = {
        .dimensions = inputDims,
        .orderOfDimensions = inputOrderOfDims,
        .numberOfDimensions = inputNumberOfDims
    };
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);
    setTensorValues(&inputFloat, inputFloatData, &inputShape, &inputFloatQ, NULL);

    tensor_t inputAsym;
    uint8_t inputAsymData[numberOfElements];
    asymQConfig_t inputAsymQConfig;
    initAsymQConfig(8, HTE, &inputAsymQConfig);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQConfig, &inputAsymQ);
    setTensorValues(&inputAsym, inputAsymData, &inputShape, &inputAsymQ, NULL);
    convertTensor(&inputFloat, &inputAsym);

    tensor_t outputFloat;
    float outputFloatData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    shape_t outputShape = {
        .dimensions = outputDims,
        .orderOfDimensions = outputOrderOfDims,
        .numberOfDimensions = outputNumberOfDims
    };
    quantization_t outputFloatQ;
    initFloat32Quantization(&outputFloatQ);
    setTensorValues(&outputFloat, outputFloatData, &outputShape, &outputFloatQ, NULL);

    tensor_t outputAsym;
    uint8_t outputAsymData[numberOfElements];
    asymQConfig_t outputAsymQConfig;
    initAsymQConfig(8, HTE, &outputAsymQConfig);
    quantization_t outputAsymQ;
    initAsymQuantization(&outputAsymQConfig, &outputAsymQ);
    setTensorValues(&outputAsym, outputAsymData, &outputShape, &outputAsymQ, NULL);
    convertTensor(&outputFloat, &outputAsym);

    layer_t reluLayer;
    initLayer(&reluLayer, RELU, NULL, ASYM_LAYER, &inputAsymQ, &outputAsymQ);
    reluForward(&reluLayer, &inputAsym, &outputAsym);

    convertTensor(&outputAsym, &outputFloat);
    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, outputFloat.data, actual);

    float expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};

    for (size_t i = 0; i < numberOfElements; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, expected[i], actual[i]);
    }
}

void testReluBackwardFloat() {
    size_t numberOfElements = 6;

    size_t dims[] = {numberOfElements};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    shape_t shape = {
        .dimensions = dims,
        .orderOfDimensions = orderOfDims,
        .numberOfDimensions = numberOfDims
    };

    tensor_t forwardInput;
    float forwardInputData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    quantization_t forwardInputQ;
    initFloat32Quantization(&forwardInputQ);
    setTensorValues(&forwardInput, forwardInputData, &shape, &forwardInputQ, NULL);

    tensor_t loss;
    float lossData[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    quantization_t lossQ;
    initFloat32Quantization(&lossQ);
    setTensorValues(&loss, lossData, &shape, &lossQ, NULL);

    tensor_t propLoss;
    float propLossData[numberOfElements];
    quantization_t propLossQ;
    initFloat32Quantization(&propLossQ);
    setTensorValues(&propLoss, propLossData, &shape, &propLossQ, NULL);

    layer_t reluLayer;
    initLayer(&reluLayer, RELU, NULL, FLOAT_LAYER, &forwardInputQ, &propLossQ);

    reluBackward(&reluLayer, &forwardInput, &loss, &propLoss);

    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, propLoss.data, actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, numberOfElements);
}

void testReluBackwardAsym() {
    size_t numberOfElements = 6;

    size_t dims[] = {numberOfElements};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    shape_t shape = {
        .dimensions = dims,
        .orderOfDimensions = orderOfDims,
        .numberOfDimensions = numberOfDims
    };

    tensor_t forwardInputFloat;
    float forwardInputData[] = {-1, 0, 1, 2, 5, -6};
    quantization_t forwardInputQ;
    initFloat32Quantization(&forwardInputQ);
    setTensorValues(&forwardInputFloat, forwardInputData, &shape, &forwardInputQ, NULL);

    tensor_t lossFloat;
    float lossFloatData[] = {0, 2, -4, 6, 3, 2};
    quantization_t lossFloatQ;
    initFloat32Quantization(&lossFloatQ);
    setTensorValues(&lossFloat, lossFloatData, &shape, &lossFloatQ, NULL);

    tensor_t propLossFloat;
    float propLossData[numberOfElements];
    quantization_t propLossQ;
    initFloat32Quantization(&propLossQ);
    setTensorValues(&propLossFloat, propLossData, &shape, &propLossQ, NULL);

    tensor_t forwardInputAsym;
    asymQConfig_t forwardInputAsymQConfig;
    initAsymQConfig(8, HTE, &forwardInputAsymQConfig);
    quantization_t forwardInputAsymQ;
    initAsymQuantization(&forwardInputAsymQConfig, &forwardInputAsymQ);
    uint8_t forwardInputDataAsym[numberOfElements * calcBytesPerElement(&forwardInputAsymQ)];
    setTensorValuesForConversion(forwardInputDataAsym, &forwardInputAsymQ, &forwardInputFloat, &forwardInputAsym);
    convertTensor(&forwardInputFloat, &forwardInputAsym);

    tensor_t lossAsym;
    asymQConfig_t lossAsymQC;
    initAsymQConfig(8, HTE, &lossAsymQC);
    quantization_t lossAsymQ;
    initAsymQuantization(&lossAsymQC, &lossAsymQ);
    uint8_t lossAsymData[numberOfElements * calcBytesPerElement(&lossAsymQ)];
    setTensorValuesForConversion(lossAsymData, &lossAsymQ, &lossFloat, &lossAsym);
    convertTensor(&lossFloat, &lossAsym);

    tensor_t propLossAsym;
    asymQConfig_t propLossAsymQConfig;
    initAsymQConfig(8, HTE, &propLossAsymQConfig);
    quantization_t propLossAsymQ;
    initAsymQuantization(&propLossAsymQConfig, &propLossAsymQ);
    uint8_t propLossDataAsym[numberOfElements*calcBytesPerElement(&propLossAsymQ)];
    setTensorValuesForConversion(propLossDataAsym, &propLossAsymQ, &propLossFloat, &propLossAsym);
    convertTensor(&propLossFloat, &propLossAsym);

    layer_t reluLayer;
    initLayer(&reluLayer, RELU, NULL, ASYM_LAYER, &forwardInputQ, &propLossAsymQ);

    reluBackward(&reluLayer, &forwardInputAsym, &lossAsym, &propLossAsym);

    convertTensor(&propLossAsym, &propLossFloat);

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, propLossFloat.data, actual);

    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    for(size_t i = 0; i < numberOfElements; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, expected[i], actual[i]);
    }
}


void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testReluForwardFloat);
    RUN_TEST(testReluForwardAsym);

    RUN_TEST(testReluBackwardFloat);
    RUN_TEST(testReluBackwardAsym);
    UNITY_END();
}
