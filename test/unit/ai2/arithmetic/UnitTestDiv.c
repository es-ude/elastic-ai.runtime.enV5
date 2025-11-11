#include "Div.h"
#include "Arithmetic.h"
#include "Tensor.h"
#include "unity.h"


void testDivFloatTensors() {
    size_t numberOfElements = 24;

    float aData[] = {1.5f, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                     22, 23, 24};
    size_t aNumberOfDims = 3;
    size_t aDims[] = {2, 3, 4};
    size_t aOrderOfDims[] = {0, 1, 2};
    quantization_t aQ;
    initFloat32Quantization(&aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, aDims, aNumberOfDims, aOrderOfDims, &aQ, NULL);

    float bData[] = {0.3f, 6, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                     22, 23, 24};
    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};

    quantization_t bQ;
    initFloat32Quantization(&bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, bDims, bNumberOfDims, bOrderOfDims, &bQ, NULL);

    transposeTensor(&bTensor, 0, 1);

    divFloat32TensorsInplace(&aTensor, &bTensor);

    float expected[] = {5.0f, 0.5f, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, aTensor.data, numberOfElements);
}

void testDivSymInt32TensorsInplace() {
    size_t numberOfValues = 6;

    int32_t aData[] = {1, 2, 3, 4, 5, 6};

    size_t dims[] = {numberOfValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    symInt32QConfig_t aSymInt32QC;
    initSymInt32QConfig(HTE, &aSymInt32QC);
    quantization_t aQ;
    initSymInt32Quantization(&aSymInt32QC, &aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, dims, numberOfDims, orderOfDims, &aQ, NULL);

    int32_t bData[] = {1, 2, 3, 4, 5, 6};

    symInt32QConfig_t bSymInt32QC;
    initSymInt32QConfig(HTE, &bSymInt32QC);
    quantization_t bQ;
    initSymInt32Quantization(&bSymInt32QC, &bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, dims, numberOfDims, orderOfDims, &bQ, NULL);

    divSymInt32TensorsInplace(&aTensor, &bTensor);

    int32_t expected[] = {1, 1, 1, 1, 1, 1};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aData, numberOfValues);

}

void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testDivFloatTensors);
    RUN_TEST(testDivSymInt32TensorsInplace);
    UNITY_END();
}
