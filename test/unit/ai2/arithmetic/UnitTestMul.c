#include "Mul.h"
#include "Arithmetic.h"
#include "Tensor.h"
#include "unity.h"


void testMulFloatTensors() {
    size_t numberOfElements = 6;

    float aData[] = {2.2f, 3.5f, 4.1f, 5.2f, 6.8f, 7.3f};
    size_t aNumberOfDims = 2;
    size_t aDims[] = {2, 3};
    size_t aOrderOfDims[] = {0, 1};
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

    float bData[] = {1.5f, 3.3f, 8.2f, 0.2f, 5.5f, 0.9f};
    size_t bNumberOfDims = 2;
    size_t bDims[] = {2, 3};
    size_t bOrderOfDims[] = {1, 0};

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

    floatElementArithmeticFunc_t mul = mulFloat32s;
    floatPointWiseArithmeticInplace(&aTensor, &bTensor, mul);

    float expected[] = {3.3f, 11.55f, 33.62f, 1.04f, 37.4f, 6.57f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, aTensor.data, numberOfElements);
}

void testMulFloatElementWithTensor() {
    size_t numberOfElements = 6;

    float aData[] = {2.2f, 3.5f, 4.1f, 5.2f, 6.8f, 7.3f};
    size_t aNumberOfDims = 2;
    size_t aDims[] = {2, 3};
    size_t aOrderOfDims[] = {0, 1};
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

    float x = 0.5;

    floatElementArithmeticFunc_t mul = mulFloat32s;
    floatElementWithTensorArithmeticInplace(&aTensor, x, mul);

    float expected[] = {1.1f, 1.75f, 2.05f, 2.6f, 3.4f, 3.65f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, aTensor.data, numberOfElements);
}


void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testMulFloatTensors);
    RUN_TEST(testMulFloatElementWithTensor);
    UNITY_END();
}
