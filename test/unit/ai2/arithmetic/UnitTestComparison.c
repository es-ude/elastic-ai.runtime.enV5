#include "Comparison.h"
#include "Arithmetic.h"
#include "Tensor.h"
#include "unity.h"

#include <DTypes.h>

void setUp() {}
void tearDown() {}

void testGTE_int32Value() {
    size_t numberOfValues = 3;
    int32_t data[] = {-1, 0, 1};
    size_t numberOfDims = 1;
    size_t dims[] = {3};
    size_t orderOfDims[] = {0};

    quantization_t quantization = {
        .type = INT32
    };

    tensor_t aTensor = {
        .data = data,
        .dimensions = dims,
        .numberOfDimensions = numberOfDims,
        .quantization = &quantization,
        .orderOfDimensions = orderOfDims,
        .sparsityBitmask = NULL
    };

    int32_t b = 0;
    int32_t altNumber = 0;

    int32_t resultData[] = {2, 2, 2};
    size_t resultNumberOfDims = 1;
    size_t resultDims[] = {3};
    size_t resultOrderOfDims[] = {0};

    quantization_t resultQuantization = {
        .type = INT32
    };

    tensor_t resultTensor = {
        .data = resultData,
        .dimensions = resultDims,
        .numberOfDimensions = resultNumberOfDims,
        .quantization = &resultQuantization,
        .orderOfDimensions = resultOrderOfDims,
        .sparsityBitmask = NULL
    };

    GTE_int32Value(&aTensor, b, altNumber, &resultTensor);

    int32_t expected[] = {0, 0, 1};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, resultTensor.data, numberOfValues);
}

void testGTE_floatTensors() {
    size_t numberOfValues = 3;
    float data[] = {-1, 0, 1};
    size_t numberOfDims = 1;
    size_t dims[] = {3};
    size_t orderOfDims[] = {0};

    quantization_t quantization = {
        .type = FLOAT32
    };

    tensor_t aTensor = {
        .data = data,
        .dimensions = dims,
        .numberOfDimensions = numberOfDims,
        .quantization = &quantization,
        .orderOfDimensions = orderOfDims,
        .sparsityBitmask = NULL
    };

    float bData[] = {-2, -1, 0};
    size_t bNumberOfDims = 1;
    size_t bDims[] = {3};
    size_t bOrderOfDims[] = {0};

    quantization_t bQuantization = {
        .type = FLOAT32
    };

    tensor_t bTensor = {
        .data = bData,
        .dimensions = bDims,
        .numberOfDimensions = bNumberOfDims,
        .quantization = &bQuantization,
        .orderOfDimensions = bOrderOfDims,
        .sparsityBitmask = NULL
    };

    float altNumber = 0;

    float resultData[] = {2, 2, 2};
    size_t resultNumberOfDims = 1;
    size_t resultDims[] = {3};
    size_t resultOrderOfDims[] = {0};

    quantization_t resultQuantization = {
        .type = FLOAT32
    };

    tensor_t resultTensor = {
        .data = resultData,
        .dimensions = resultDims,
        .numberOfDimensions = resultNumberOfDims,
        .quantization = &resultQuantization,
        .orderOfDimensions = resultOrderOfDims,
        .sparsityBitmask = NULL
    };

    GTE_floatTensor(&aTensor, &bTensor, altNumber, &resultTensor);

    float expected[] = {-1, 0, 0};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, resultTensor.data, numberOfValues);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testGTE_int32Value);
    RUN_TEST(testGTE_floatTensors);
    UNITY_END();
}
