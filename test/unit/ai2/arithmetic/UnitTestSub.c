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
    quantization_t aQ = {
        .type = INT32
    };


    tensor_t aTensor = {
        .data = aData,
        .numberOfDimensions = aNumberOfDims,
        .dimensions = aDims,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = aOrderOfDims
    };

    int32_t bData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};


    quantization_t bQ = {
        .type = INT32
    };
    tensor_t bTensor = {
        .data = bData,
        .numberOfDimensions = bNumberOfDims,
        .dimensions = bDims,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions =  bOrderOfDims
    };

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
    quantization_t aQ = {
        .type = INT32
    };


    tensor_t aTensor = {
        .data = aData,
        .numberOfDimensions = aNumberOfDims,
        .dimensions = aDims,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = aOrderOfDims
    };

    int32_t x = 5;

    int32ElementArithmeticFunc_t sub = subInt32s;
    int32ElementWithTensorArithmeticInplace(&aTensor, x, sub);

    int32_t expected[] = {-4, -3, -2, -1, 0, 1};



    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aTensor.data, numberOfElements);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testSubInt32Tensors);
    RUN_TEST(testSubInt32ElementWithTensor);
    UNITY_END();
}