#include "Matmul.h"
#include "Arithmetic.h"
#include "Tensor.h"
#include "unity.h"

#include <DTypes.h>


void testMatmulInt32() {
    size_t numberOfElements = 6;

    /*
    1, 2, 3,
    4, 5, 6
    */
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

    /*
    1, 4,
    2, 5,
    3, 6
    */
    int32_t bData[] = {1, 4, 2, 5, 3, 6};
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
        .orderOfDimensions = bOrderOfDims
    };

    int32_t outputData[] = {0, 0, 0, 0};
    size_t outputNumberOfDims = 2;
    size_t outputDims[] = {2, 2};
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

    matmulInt32Tensors(&aTensor, &bTensor, &outputTensor);

    int32_t expected[] = {14, 32, 32, 77};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, outputTensor.data, 4);
}

void testMatmulInt32WithVector() {
    /*
    1, 2, 3,
    4, 5, 6
    */
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

    /*
    1,
    2,
    3
    */
    int32_t bData[] = {1, 2, 3};
    size_t bNumberOfDims = 1;
    size_t bDims[] = {3};
    size_t bOrderOfDims[] = {0};
    quantization_t bQ = {
        .type = INT32
    };

    tensor_t bTensor = {
        .data = bData,
        .numberOfDimensions = bNumberOfDims,
        .dimensions = bDims,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = bOrderOfDims
    };

    int32_t outputData[] = {0, 0};
    size_t outputNumberOfDims = 1;
    size_t outputDims[] = {2};
    size_t outputOrderOfDims[] = {0};
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

    matmulInt32Tensors(&aTensor, &bTensor, &outputTensor);

    int32_t expected[] = {14, 32};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, outputTensor.data, 2);
}

void testMatmulFloatVectors() {
    /*
    1.1, 2.4, 3.9,
    */
    float aData[] = {1.1f, 2.4f, 3.9f};
    size_t aNumberOfDims = 1;
    size_t aDims[] = {3};
    size_t aOrderOfDims[] = {0};
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

    /*
    1.5,
    2.9,
    3.3
    */
    float bData[] = {1.5f, 2.9f, 3.3f};
    size_t bNumberOfDims = 1;
    size_t bDims[] = {3};
    size_t bOrderOfDims[] = {0};
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

    float outputData[] = {0};
    size_t outputNumberOfDims = 1;
    size_t outputDims[] = {1};
    size_t outputOrderOfDims[] = {0};
    quantization_t outputQ = {
        .type = FLOAT32
    };

    tensor_t outputTensor = {
        .data = outputData,
        .numberOfDimensions = outputNumberOfDims,
        .dimensions = outputDims,
        .quantization = &outputQ,
        .sparsityBitmask = NULL,
        .orderOfDimensions = outputOrderOfDims
    };

    matmulFloatTensors(&aTensor, &bTensor, &outputTensor);

    float expected[] = {21.48f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, outputTensor.data, 1);
}


void setUp() {}
void tearDown() {}

int main(void) {

    UNITY_BEGIN();
    RUN_TEST(testMatmulInt32);
    RUN_TEST(testMatmulInt32WithVector);
    RUN_TEST(testMatmulFloatVectors);
    UNITY_END();
}
