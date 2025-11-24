#include "Sub.h"
#include "Tensor.h"
#include "Arithmetic.h"
#include "unity.h"

void setUp(){}
void tearDown(){}

void testSubInt32Tensors() {
    size_t numberOfElements = 24;

    int32_t aData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    size_t aNumberOfDims = 3;
    size_t aDims[] = {2, 3, 4};
    size_t aOrderOfDims[] = {0, 1, 2};
    shape_t aShape = {
        .dimensions = aDims,
        .orderOfDimensions = aOrderOfDims,
        .numberOfDimensions = aNumberOfDims
    };
    quantization_t aQ;
    initInt32Quantization(&aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, &aShape, &aQ, NULL);

    int32_t bData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};
    shape_t bShape = {
        .dimensions = bDims,
        .orderOfDimensions = bOrderOfDims,
        .numberOfDimensions = bNumberOfDims
    };

    quantization_t bQ;
    initInt32Quantization(&bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, &bShape, &bQ, NULL);

    transposeTensor(&bTensor, 0, 1);

    int32ElementArithmeticFunc_t sub = subInt32s;
    int32PointWiseArithmeticInplace(&aTensor, &bTensor, sub);

    int32_t expected[24] = {0};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aTensor.data, numberOfElements);
}

void testSubInt32ElementWithTensor() {
    size_t numberOfElements = 6;

    int32_t aData[] = {1, 2, 3, 4, 5, 6};
    size_t aNumberOfDims = 2;
    size_t aDims[] = {2, 3};
    size_t aOrderOfDims[] = {0, 1};
    shape_t aShape = {
        .dimensions = aDims,
        .orderOfDimensions = aOrderOfDims,
        .numberOfDimensions = aNumberOfDims
    };
    quantization_t aQ;
    initInt32Quantization(&aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, &aShape, &aQ, NULL);

    int32_t x = 5;

    int32ElementArithmeticFunc_t sub = subInt32s;
    int32ElementWithTensorArithmeticInplace(&aTensor, x, sub);

    int32_t expected[] = {-4, -3, -2, -1, 0, 1};



    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aTensor.data, numberOfElements);
}

void testSubSymInt32Tensors() {
    size_t numberOfElements = 24;

    int32_t aData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

    size_t aNumberOfDims = 3;
    size_t aDims[] = {2, 3, 4};
    size_t aOrderOfDims[] = {0, 1, 2};
    shape_t aShape = {
        .dimensions = aDims,
        .orderOfDimensions = aOrderOfDims,
        .numberOfDimensions = aNumberOfDims
    };
    symInt32QConfig_t aSymInt32QC;
    initSymInt32QConfig(HTE, &aSymInt32QC);
    quantization_t aQ;
    initSymInt32Quantization(&aSymInt32QC, &aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, &aShape, &aQ, NULL);

    int32_t bData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};
    shape_t bShape = {
        .dimensions = bDims,
        .orderOfDimensions = bOrderOfDims,
        .numberOfDimensions = bNumberOfDims
    };

    symInt32QConfig_t bSymInt32QC;
    initSymInt32QConfig(HTE, &bSymInt32QC);
    quantization_t bQ;
    initSymInt32Quantization(&bSymInt32QC, &bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, &bShape, &bQ, NULL);

    transposeTensor(&bTensor, 0, 1);

    int32ElementArithmeticFunc_t sub = subInt32s;
    int32PointWiseArithmeticInplace(&aTensor, &bTensor, sub);

    int32_t expected[24] = {0};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aTensor.data, numberOfElements);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testSubInt32Tensors);
    RUN_TEST(testSubInt32ElementWithTensor);
    RUN_TEST(testSubSymInt32Tensors);
    UNITY_END();
}