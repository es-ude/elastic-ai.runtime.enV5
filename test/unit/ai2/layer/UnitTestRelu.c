#include "Relu.h"
#include "Quantization.h"
#include "unity.h"

#include <DTypes.h>
#include <Tensor.h>

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

void testReluForwardInt32() {
    size_t numberOfElements = 6;

    int32_t inputData[] = {-1, 0, 1, 2, 5, -6};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    quantization_t inputQ;
    initInt32Quantization(&inputQ);

    tensor_t inputTensor;
    setTensorValues(&inputTensor, inputData, inputDims, inputNumberOfDims, inputOrderOfDims,
                    &inputQ, NULL);

    int32_t outputData[numberOfElements];
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    quantization_t outputQ;
    initInt32Quantization(&outputQ);

    tensor_t outputTensor;
    setTensorValues(&outputTensor, outputData, outputDims, outputNumberOfDims, outputOrderOfDims,
                    &outputQ, NULL);

    reluForward(NULL, &inputTensor, &outputTensor);

    int32_t expected[] = {0, 0, 1, 2, 5, 0};

    int32_t actual[numberOfElements];
    readBytesAsInt32Array(numberOfElements, outputTensor.data, actual);

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, numberOfElements);
}

void testReluForwardLinear() {
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

    tensor_t inputLinearTensor;
    uint8_t inputLinearData[numberOfElements];
    linearQConfig_t inputLinearQConfig;
    initLinearQConfig(8, HTE, &inputLinearQConfig);
    quantization_t inputLinearQ;
    initLinearQuantization(&inputLinearQConfig, &inputLinearQ);

    setTensorValues(&inputLinearTensor, inputLinearData, inputDims, inputNumberOfDims,
                    inputOrderOfDims, &inputLinearQ, NULL);

    convertTensor(&inputFloatTensor, &inputLinearTensor);

    float outputFloatData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    quantization_t outputFloatQ;
    initFloat32Quantization(&outputFloatQ);

    tensor_t outputFloatTensor;
    setTensorValues(&outputFloatTensor, outputFloatData, outputDims, outputNumberOfDims,
                    outputOrderOfDims, &outputFloatQ, NULL);

    uint8_t outputLinearData[numberOfElements];
    linearQConfig_t outputLinearQConfig;
    initLinearQConfig(8, HTE, &outputLinearQConfig);
    quantization_t outputLinearQ;
    initLinearQuantization(&outputLinearQConfig, &outputLinearQ);

    tensor_t outputLinearTensor;
    setTensorValues(&outputLinearTensor, outputLinearData, outputDims, outputNumberOfDims,
                    outputOrderOfDims, &outputLinearQ, NULL);

    convertTensor(&outputFloatTensor, &outputLinearTensor);

    reluForward(NULL, &inputLinearTensor, &outputLinearTensor);

    convertTensor(&outputLinearTensor, &outputFloatTensor);
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

    float gradData[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    quantization_t gradQ;
    initFloat32Quantization(&gradQ);
    tensor_t grad;
    setTensorValues(&grad, gradData, dims, numberOfDims, orderOfDims, &gradQ, NULL);

    float outputData[numberOfElements];
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    tensor_t output;
    setTensorValues(&output, outputData, dims, numberOfDims, orderOfDims, &outputQ, NULL);

    reluBackward(NULL, &input, &grad, &output);

    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, output.data, actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, numberOfElements);
}

void testReluBackwardInt32() {
    size_t numberOfElements = 6;

    size_t dims[] = {numberOfElements};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    int32_t inputData[] = {-1, 0, 1, 2, 5, -6};
    quantization_t inputQ;
    initInt32Quantization(&inputQ);
    tensor_t input;
    setTensorValues(&input, inputData, dims, numberOfDims, orderOfDims, &inputQ, NULL);

    int32_t gradData[] = {0, 2, -4, 6, 3, 2};
    quantization_t gradQ;
    initInt32Quantization(&gradQ);
    tensor_t grad;
    setTensorValues(&grad, gradData, dims, numberOfDims, orderOfDims, &gradQ, NULL);

    int32_t outputData[numberOfElements];
    quantization_t outputQ;
    initInt32Quantization(&outputQ);
    tensor_t output;
    setTensorValues(&output, outputData, dims, numberOfDims, orderOfDims, &outputQ, NULL);

    reluBackward(NULL, &input, &grad, &output);

    int32_t expected[] = {0, 0, -4, 6, 3, 0};

    int32_t actual[numberOfElements];
    readBytesAsInt32Array(numberOfElements, output.data, actual);

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, numberOfElements);
}

void testReluBackwardLinear() {
    size_t numberOfElements = 6;

    size_t dims[] = {numberOfElements};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    float inputData[] = {-1, 0, 1, 2, 5, -6};
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    tensor_t input;
    setTensorValues(&input, inputData, dims, numberOfDims, orderOfDims, &inputQ, NULL);

    float gradData[] = {0, 2, -4, 6, 3, 2};
    quantization_t gradQ;
    initFloat32Quantization(&gradQ);
    tensor_t grad;
    setTensorValues(&grad, gradData, dims, numberOfDims, orderOfDims, &gradQ, NULL);

    float outputData[numberOfElements];
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    tensor_t output;
    setTensorValues(&output, outputData, dims, numberOfDims, orderOfDims, &outputQ, NULL);

    uint8_t inputDataLinear[numberOfElements];
    linearQConfig_t inputLinearQConfig;
    initLinearQConfig(8, HTE, &inputLinearQConfig);
    quantization_t inputLinearQ;
    initLinearQuantization(&inputLinearQConfig, &inputLinearQ);
    tensor_t inputLinear;
    setTensorValuesForConversion(inputDataLinear, &inputLinearQ, &input, &inputLinear);
    convertTensor(&input, &inputLinear);

    uint8_t gradDataLinear[numberOfElements];
    linearQConfig_t gradLinearQConfig;
    initLinearQConfig(8, HTE, &gradLinearQConfig);
    quantization_t gradLinearQ;
    initLinearQuantization(&gradLinearQConfig, &gradLinearQ);
    tensor_t gradLinear;
    setTensorValuesForConversion(gradDataLinear, &gradLinearQ, &grad, &gradLinear);
    convertTensor(&grad, &gradLinear);

    uint8_t outputDataLinear[numberOfElements];
    linearQConfig_t outputLinearQConfig;
    initLinearQConfig(8, HTE, &outputLinearQConfig);
    quantization_t outputLinearQ;
    initLinearQuantization(&outputLinearQConfig, &outputLinearQ);
    tensor_t outputLinear;
    setTensorValuesForConversion(outputDataLinear, &outputLinearQ, &output, &outputLinear);
    convertTensor(&output, &outputLinear);

    reluBackward(NULL, &inputLinear, &gradLinear, &outputLinear);


    convertTensor(&outputLinear, &output);

    float actual[numberOfElements];
    readBytesAsFloatArray(numberOfElements, output.data, actual);

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
    RUN_TEST(testReluForwardInt32);
    RUN_TEST(testReluForwardLinear);

    RUN_TEST(testReluBackwardFloat);
    RUN_TEST(testReluBackwardInt32);
    RUN_TEST(testReluBackwardLinear);
    UNITY_END();
}
