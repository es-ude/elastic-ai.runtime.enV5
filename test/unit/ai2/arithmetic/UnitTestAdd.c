#include "Add.h"
#include "Tensor.h"
#include "Arithmetic.h"
#include "unity.h"

#include <DTypes.h>

void setUp(){}
void tearDown(){}

void testAddInt32Tensors() {
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

    int32_t bData[] = {1, 2, 3, 4, 5, 6};
    size_t bNumberOfDims = 2;
    size_t bDims[] = {3, 2};
    size_t bOrderOfDims[] = {0, 1};


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

    int32_t outputData[] = {0, 0, 0, 0, 0, 0};
    size_t outputNumberOfDims = 2;
    size_t outputDims[] = {2, 3};
    size_t outputOrderOfDims[] = {0, 1};

    quantization_t outputQ = {
        .type = INT32
    };
    tensor_t outputTensor = {
        .data = outputData,
        .numberOfDimensions = outputNumberOfDims,
        .dimensions = outputDims,
        .quantization = &outputQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = outputOrderOfDims
    };

    transposeTensor(&bTensor, 0, 1);

    int32ElementArithmeticFunc_t add = addInt32s;
    int32PointWiseArithmetic(&aTensor, &bTensor, add, &outputTensor);

    int32_t expected[] = {2, 4, 6, 8, 10, 12};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, outputTensor.data, numberOfElements);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testAddInt32Tensors);
    UNITY_END();
}