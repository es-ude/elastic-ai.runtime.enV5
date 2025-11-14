#include "Matmul.h"
#include "Arithmetic.h"
#include "Tensor.h"
#include "unity.h"

#include <DTypes.h>
#include <TensorConversion.h>


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
        .shape.numberOfDimensions = aNumberOfDims,
        .shape.dimensions = aDims,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = aOrderOfDims
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
        .shape.numberOfDimensions = bNumberOfDims,
        .shape.dimensions = bDims,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = bOrderOfDims
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
        .shape.numberOfDimensions = outputNumberOfDims,
        .shape.dimensions = outputDims,
        .quantization = &outputQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = outputOrderOfDims
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
        .shape.numberOfDimensions = aNumberOfDims,
        .shape.dimensions = aDims,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = aOrderOfDims
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
        .shape.numberOfDimensions = bNumberOfDims,
        .shape.dimensions = bDims,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = bOrderOfDims
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
        .shape.numberOfDimensions = outputNumberOfDims,
        .shape.dimensions = outputDims,
        .quantization = &outputQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = outputOrderOfDims
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
        .shape.numberOfDimensions = aNumberOfDims,
        .shape.dimensions = aDims,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = aOrderOfDims
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
        .shape.numberOfDimensions = bNumberOfDims,
        .shape.dimensions = bDims,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = bOrderOfDims
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
        .shape.numberOfDimensions = outputNumberOfDims,
        .shape.dimensions = outputDims,
        .quantization = &outputQ,
        .sparsityBitmask = NULL,
        .shape.orderOfDimensions = outputOrderOfDims
    };

    matmulFloat32Tensors(&aTensor, &bTensor, &outputTensor);

    float expected[] = {21.48f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, outputTensor.data, 1);
}

void testMatmulSymInt32Tensors() {

    int32_t aData[] = {1, 2, 3, 4, 5, 6};

    size_t aNumberOfDims = 2;
    size_t aDims[] = {2, 3};
    size_t aOrderOfDims[] = {0, 1};

    symInt32QConfig_t aSymInt32QC;
    initSymInt32QConfig(HTE, &aSymInt32QC);
    aSymInt32QC.scale = 2.f;
    quantization_t aQ;
    initSymInt32Quantization(&aSymInt32QC, &aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, aDims, aNumberOfDims, aOrderOfDims, &aQ, NULL);

    int32_t bData[] = {1, 4, 2, 5, 3, 6};

    size_t bNumberOfDims = 2;
    size_t bDims[] = {3, 2};
    size_t bOrderOfDims[] = {0, 1};


    symInt32QConfig_t bSymInt32QC;
    initSymInt32QConfig(HTE, &bSymInt32QC);
    quantization_t bQ;
    initSymInt32Quantization(&bSymInt32QC, &bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, bDims, bNumberOfDims, bOrderOfDims, &bQ, NULL);

    int32_t outputData[4];

    size_t outputNumberOfDims = 2;
    size_t outputDims[] = {2, 2};
    size_t outputOrderOfDims[] = {0, 1};

    symInt32QConfig_t outputSymInt32QC;
    initSymInt32QConfig(HTE, &outputSymInt32QC);
    quantization_t outputQ;
    initSymInt32Quantization(&outputSymInt32QC, &outputQ);

    tensor_t outputTensor;
    setTensorValues(&outputTensor, outputData, outputDims, outputNumberOfDims, outputOrderOfDims, &outputQ, NULL);

    matmulSymInt32Tensors(&aTensor, &bTensor, &outputTensor);

    float expected[] = {28.f, 64.f, 64.f, 154.f};

    float actualData[4];
    quantization_t actualQ;
    initFloat32Quantization(&actualQ);

    tensor_t actual;
    setTensorValues(&actual, actualData, outputDims, outputNumberOfDims, outputOrderOfDims, &actualQ, NULL);
    convertTensor(&outputTensor, &actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual.data, 4);
}

void setUp() {}
void tearDown() {}

int main(void) {

    UNITY_BEGIN();
    RUN_TEST(testMatmulInt32);
    RUN_TEST(testMatmulInt32WithVector);
    RUN_TEST(testMatmulFloatVectors);
    RUN_TEST(testMatmulSymInt32Tensors);
    UNITY_END();
}
