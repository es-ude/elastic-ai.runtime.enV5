#include "Relu.h"
#include "Quantization.h"
#include "unity.h"
#include "DTypes.h"
#include "Tensor.h"
#include "TensorConversion.h"

void testReluForwardFloat() {
    size_t numberOfElements = 6;

    float inputData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);

    tensor_t inputTensor;
    setTensorValues(&inputTensor, inputData, inputDims, inputNumberOfDims, inputOrderOfDims,
                    &inputQ, NULL);

    float outputData[numberOfElements];
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);

    tensor_t outputTensor;
    setTensorValues(&outputTensor, outputData, outputDims, outputNumberOfDims, outputOrderOfDims,
                    &outputQ, NULL);

    reluForward(NULL, &inputTensor, &outputTensor);

    float expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, outputTensor.data, actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, numberOfElements);
}

void testReluForwardAsym() {
    size_t numberOfElements = 6;

    float inputFloatData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);
    tensor_t inputFloatTensor;
    setTensorValues(&inputFloatTensor, inputFloatData, inputDims, inputNumberOfDims,
                    inputOrderOfDims, &inputFloatQ, NULL);

    tensor_t inputAsymTensor;
    uint8_t inputAsymData[numberOfElements];
    asymQConfig_t inputAsymQConfig;
    initAsymQConfig(8, HTE, &inputAsymQConfig);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQConfig, &inputAsymQ);
    setTensorValues(&inputAsymTensor, inputAsymData, inputDims, inputNumberOfDims,
                    inputOrderOfDims, &inputAsymQ, NULL);
    convertTensor(&inputFloatTensor, &inputAsymTensor);

    float outputFloatData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    quantization_t outputFloatQ;
    initFloat32Quantization(&outputFloatQ);
    tensor_t outputFloatTensor;
    setTensorValues(&outputFloatTensor, outputFloatData, outputDims, outputNumberOfDims,
                    outputOrderOfDims, &outputFloatQ, NULL);

    uint8_t outputAsymData[numberOfElements];
    asymQConfig_t outputAsymQConfig;
    initAsymQConfig(8, HTE, &outputAsymQConfig);
    quantization_t outputAsymQ;
    initAsymQuantization(&outputAsymQConfig, &outputAsymQ);
    tensor_t outputAsymTensor;
    setTensorValues(&outputAsymTensor, outputAsymData, outputDims, outputNumberOfDims,
                    outputOrderOfDims, &outputAsymQ, NULL);
    convertTensor(&outputFloatTensor, &outputAsymTensor);

    reluForward(NULL, &inputAsymTensor, &outputAsymTensor);

    convertTensor(&outputAsymTensor, &outputFloatTensor);
    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, outputFloatTensor.data, actual);

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

    float inputData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    tensor_t input;
    setTensorValues(&input, inputData, dims, numberOfDims, orderOfDims, &inputQ, NULL);

    tensor_t gradOutputFromPreviousLayer;
    float gradOutputData[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    quantization_t gradOutputQ;
    initFloat32Quantization(&gradOutputQ);
    setTensorValues(&gradOutputFromPreviousLayer, gradOutputData, dims, numberOfDims, orderOfDims, &gradOutputQ, NULL);

    tensor_t gradInputForNextLayer;
    float gradInputData[numberOfElements];
    quantization_t gradInputQ;
    initFloat32Quantization(&gradInputQ);
    setTensorValues(&gradInputForNextLayer, gradInputData, dims, numberOfDims, orderOfDims, &inputQ, NULL);

    reluBackward(NULL, &input, &gradOutputFromPreviousLayer, &gradInputForNextLayer);

    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, gradInputForNextLayer.data, actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, numberOfElements);
}

void testReluBackwardAsym() {
    size_t numberOfElements = 6;

    size_t dims[] = {numberOfElements};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    tensor_t inputFloat;
    float inputData[] = {-1, 0, 1, 2, 5, -6};
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&inputFloat, inputData, dims, numberOfDims, orderOfDims, &inputQ, NULL);

    tensor_t gradOutputFromPreviousLayerFloat;
    float gradOutputData[] = {0, 2, -4, 6, 3, 2};
    quantization_t gradOutputQ;
    initFloat32Quantization(&gradOutputQ);
    setTensorValues(&gradOutputFromPreviousLayerFloat, gradOutputData, dims, numberOfDims, orderOfDims, &gradOutputQ, NULL);

    tensor_t gradInputForNextLayerFloat;
    float gradInputData[numberOfElements];
    quantization_t gradInputQ;
    initFloat32Quantization(&gradInputQ);
    setTensorValues(&gradInputForNextLayerFloat, gradInputData, dims, numberOfDims, orderOfDims, &gradInputQ, NULL);

    tensor_t inputAsym;
    asymQConfig_t inputAsymQConfig;
    initAsymQConfig(8, HTE, &inputAsymQConfig);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQConfig, &inputAsymQ);
    uint8_t inputDataAsym[numberOfElements * calcBytesPerElement(&inputAsymQ)];
    setTensorValuesForConversion(inputDataAsym, &inputAsymQ, &inputAsym, &inputAsym);
    convertTensor(&inputFloat, &inputAsym);

    tensor_t gradOutputFromPreviousLayerAsym;
    asymQConfig_t gradOutputAsymQC;
    initAsymQConfig(8, HTE, &gradOutputAsymQC);
    quantization_t gradOutputAsymQ;
    initAsymQuantization(&gradOutputAsymQC, &gradOutputAsymQ);
    uint8_t gradOutputAsymData[numberOfElements * calcBytesPerElement(&gradOutputAsymQ)];
    setTensorValuesForConversion(gradOutputAsymData, &gradOutputAsymQ, &gradOutputFromPreviousLayerFloat, &gradOutputFromPreviousLayerAsym);
    convertTensor(&gradOutputFromPreviousLayerFloat, &gradOutputFromPreviousLayerAsym);

    tensor_t gradInputForNextLayerAsym;
    asymQConfig_t gradInputAsymQConfig;
    initAsymQConfig(8, HTE, &gradInputAsymQConfig);
    quantization_t gradInputAsymQ;
    initAsymQuantization(&gradInputAsymQConfig, &gradInputAsymQ);
    uint8_t gradInputDataAsym[numberOfElements*calcBytesPerElement(&gradInputAsymQ)];
    setTensorValuesForConversion(gradInputDataAsym, &gradInputAsymQ, &gradInputForNextLayerFloat, &gradInputForNextLayerAsym);
    convertTensor(&gradInputForNextLayerFloat, &gradInputForNextLayerAsym);

    reluBackward(NULL, &inputAsym, &gradOutputFromPreviousLayerAsym, &gradInputForNextLayerAsym);

    convertTensor(&gradInputForNextLayerAsym, &gradInputForNextLayerFloat);

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, gradInputForNextLayerFloat.data, actual);

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
