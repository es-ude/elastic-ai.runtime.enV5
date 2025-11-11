#include "Comparison.h"
#include "Arithmetic.h"
#include "Tensor.h"
#include "unity.h"

#include <DTypes.h>

void setUp() {}
void tearDown() {}

void testGteInt32Value() {
    size_t numberOfValues = 3;

    int32_t aData[] = {-1, 0, 1};

    size_t numberOfDims = 1;
    size_t dims[] = {3};
    size_t orderOfDims[] = {0};

    quantization_t aQ;
    initInt32Quantization(&aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, dims, numberOfDims, orderOfDims, &aQ, NULL);

    int32_t b = 0;
    int32_t altNumber = 0;

    int32_t resultData[numberOfValues];

    quantization_t resultQ;
    initInt32Quantization(&resultQ);

    tensor_t resultTensor;
    setTensorValues(&resultTensor, resultData, dims, numberOfDims, orderOfDims, &resultQ, NULL);

    gteInt32Value(&aTensor, b, altNumber, &resultTensor);

    int32_t expected[] = {0, 0, 1};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, resultTensor.data, numberOfValues);
}

void testGteFloatTensors() {
    size_t numberOfValues = 3;

    float aData[] = {-1, -2, 1};

    size_t numberOfDims = 1;
    size_t dims[] = {3};
    size_t orderOfDims[] = {0};

    quantization_t aQ;
    initFloat32Quantization(&aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, dims, numberOfDims, orderOfDims, &aQ, NULL);

    float bData[] = {-2, -1, 0};

    quantization_t bQ;
    initFloat32Quantization(&bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, dims, numberOfDims, orderOfDims, &bQ, NULL);

    float altNumber = 0;

    float resultData[numberOfValues];

    quantization_t resultQ;
    initFloat32Quantization(&resultQ);

    tensor_t resultTensor;
    setTensorValues(&resultTensor, resultData, dims, numberOfDims, orderOfDims, &resultQ, NULL);

    gteFloatTensor(&aTensor, &bTensor, altNumber, &resultTensor);

    float expected[] = {-1, 0, 1};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, resultTensor.data, numberOfValues);
}

void testGteSymInt32Value() {
    size_t numberOfValues = 4;

    int32_t aData[] = {-1, 2, -3, 4};

    size_t dims[] = {numberOfValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    symInt32QConfig_t aSymInt32QC;
    initSymInt32QConfig(HTE, &aSymInt32QC);
    quantization_t aQ;
    initSymInt32Quantization(&aSymInt32QC, &aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, dims, numberOfDims, orderOfDims, &aQ, NULL);

    int32_t x = 1;
    int32_t altNumber = 5;

    int32_t resultData[numberOfValues];

    symInt32QConfig_t resultSymInt32QC;
    initSymInt32QConfig(HTE, &resultSymInt32QC);
    quantization_t resultQ;
    initSymInt32Quantization(&resultSymInt32QC, &resultQ);

    tensor_t resultTensor;
    setTensorValues(&resultTensor, resultData, dims, numberOfDims, orderOfDims, &resultQ, NULL);

    gteSymInt32Value(&aTensor, x, altNumber, &resultTensor);

    int32_t expected[] = {5, 2, 5, 4};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, resultData, numberOfValues);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testGteInt32Value);
    RUN_TEST(testGteFloatTensors);
    RUN_TEST(testGteSymInt32Value);

    UNITY_END();
}
