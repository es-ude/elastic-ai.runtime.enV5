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
    quantization_t aQ = {
        .type = FLOAT32
    };

    tensor_t aTensor = {
        .data = aData,
        .numberOfDimensions = aNumberOfDims,
        .dimensions = aDims,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = aOrderOfDims
    };

    float bData[] = {0.3f, 6, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                     22, 23, 24};
    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};

    quantization_t bQ = {
        .type = FLOAT32
    };
    tensor_t bTensor = {
        .data = bData,
        .numberOfDimensions = bNumberOfDims,
        .dimensions = bDims,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = bOrderOfDims
    };

    transposeTensor(&bTensor, 0, 1);

    floatElementArithmeticFunc_t div = divFloats;
    floatPointWiseArithmeticInplace(&aTensor, &bTensor, div);

    float expected[] = {5.0f, 0.5f, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, aTensor.data, numberOfElements);
}


void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testDivFloatTensors);
    UNITY_END();
}
