#include "Add.h"
#include "Tensor.h"
#include "unity.h"

#include <DTypes.h>

void setUp(){}
void tearDown(){}

void testAddInt32TensorsInplace() {
    size_t numberOfElements = 24;

    int32_t aData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    size_t aNumberOfDims = 3;
    size_t aDims[] = {2, 3, 4};
    size_t aOrderOfDims[] = {0, 1, 2};
    quantization_t aQ = {
        .type = INT32
    };

    shape_t aShape = {
        .dimensions = aDims,
        .numberOfDimensions = aNumberOfDims,
        .orderOfDimensions = aOrderOfDims
    };


    tensor_t aTensor = {
        .data = aData,
        .shape = &aShape,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
    };

    int32_t bData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};

    shape_t bShape = {
        .dimensions = bDims,
        .numberOfDimensions = bNumberOfDims,
        .orderOfDimensions = bOrderOfDims
    };

    quantization_t bQ = {
        .type = INT32
    };
    tensor_t bTensor = {
        .data = bData,
        .shape = &bShape,
        .quantization = &bQ,
        .sparsityBitmask = NULL,
    };

    transposeTensor(&bTensor, 0, 1);

    addInt32TensorsInplace(&aTensor, &bTensor);

    int32_t expected[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aTensor.data, numberOfElements);
}

void testAddInt32ElementWithInt32TensorInplace() {
    size_t numberOfElements = 6;

    int32_t aData[] = {1, 2, 3, 4, 5, 6};
    size_t aNumberOfDims = 2;
    size_t aDims[] = {2, 3};
    size_t aOrderOfDims[] = {0, 1};
    quantization_t aQ = {
        .type = INT32
    };


    shape_t aShape = {
        .dimensions = aDims,
        .numberOfDimensions = aNumberOfDims,
        .orderOfDimensions = aOrderOfDims
    };


    tensor_t aTensor = {
        .data = aData,
        .shape = &aShape,
        .quantization = &aQ,
        .sparsityBitmask = NULL,
    };

    int32_t x = 5;

    addInt32ElementWithInt32TensorInplace(&aTensor, x);

    int32_t expected[] = {6, 7, 8, 9, 10, 11};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aTensor.data, numberOfElements);
}

void testAddFloat32TensorsInplace() {
    size_t numberOfElements = 24;

    float aData[] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f, 17.f, 18.f, 19.f, 20.f, 21.f, 22.f, 23.f};
    size_t aNumberOfDims = 3;
    size_t aDims[] = {2, 3, 4};
    size_t aOrderOfDims[] = {0, 1, 2};
    shape_t aShape = {
        .dimensions = aDims,
        .numberOfDimensions = aNumberOfDims,
        .orderOfDimensions = aOrderOfDims
    };

    quantization_t aQ;
    initFloat32Quantization(&aQ);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, &aShape, &aQ, NULL);


    float bData[] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f, 17.f, 18.f, 19.f, 20.f, 21.f, 22.f, 23.f};
    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 3, 4};
    size_t bOrderOfDims[] = {1, 0, 2};
    shape_t bShape = {
        .dimensions = bDims,
        .numberOfDimensions = bNumberOfDims,
        .orderOfDimensions = bOrderOfDims
    };

    quantization_t bQ;
    initFloat32Quantization(&bQ);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, &bShape, &bQ, NULL);

    transposeTensor(&bTensor, 0, 1);

    addFloat32TensorsInplace(&aTensor, &bTensor);

    float expected[] = {0.f, 2.f, 4.f, 6.f, 8.f, 10.f, 12.f, 14.f, 16.f, 18.f, 20.f, 22.f, 24.f, 26.f, 28.f, 30.f, 32.f, 34.f, 36.f, 38.f, 40.f, 42.f, 44.f, 46.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, aTensor.data, numberOfElements);
}

void testAddSymInt32TensorsInplaceWithSameScale() {
    size_t numberOfValues = 6;

    symInt32QConfig_t aQC;
    initSymInt32QConfig(HTE, &aQC);
    quantization_t aQ;
    initSymInt32Quantization(&aQC, &aQ);

    int32_t aData[] = {1, 2, 3, 4, 5, 6};
    tensor_t aTensor;

    size_t dims[] = {numberOfValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    shape_t shape = {
        .dimensions = dims,
        .numberOfDimensions = numberOfDims,
        .orderOfDimensions = orderOfDims
    };

    setTensorValues(&aTensor, aData, &shape, &aQ, NULL);


    addSymInt32TensorsInplace(&aTensor, &aTensor);

    int32_t actual[numberOfValues];
    readBytesAsInt32Array(numberOfValues, aTensor.data, actual);

    int32_t expected[] = {2, 4, 6, 8, 10, 12};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, numberOfValues);
}

void testAddSymInt32TensorsInplaceWithDifferentScale() {
    size_t numberOfValues = 6;

    symInt32QConfig_t aQC;
    initSymInt32QConfig(HTE, &aQC);
    aQC.scale = 1.f;
    quantization_t aQ;
    initSymInt32Quantization(&aQC, &aQ);

    int32_t aData[] = {1, 2, 3, 4, 5, 6};
    tensor_t aTensor;

    size_t dims[] = {numberOfValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    shape_t shape = {
        .dimensions = dims,
        .numberOfDimensions = numberOfDims,
        .orderOfDimensions = orderOfDims
    };

    setTensorValues(&aTensor, aData, &shape, &aQ, NULL);

    symInt32QConfig_t bQC;
    initSymInt32QConfig(HTE, &bQC);
    bQC.scale = 2.f;
    quantization_t bQ;
    initSymInt32Quantization(&bQC, &bQ);

    int32_t bData[] = {1, 2, 3, 4, 5, 6};
    tensor_t bTensor;

    setTensorValues(&bTensor, bData, &shape, &bQ, NULL);

    addSymInt32TensorsInplace(&aTensor, &bTensor);

    int32_t actual[numberOfValues];
    readBytesAsInt32Array(numberOfValues, aTensor.data, actual);

    int32_t expected[] = {3, 6, 9, 12, 15, 18};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, numberOfValues);
}

void testAddInt32TensorWithSymInt32TensorInplace() {
    size_t numberOfValues = 6;

    symInt32QConfig_t aQC;
    initSymInt32QConfig(HTE, &aQC);
    aQC.scale = 100.f;
    quantization_t aQ;
    initSymInt32Quantization(&aQC, &aQ);
    int32_t aData[] = {1, 2, 3, 4, 5, 6};
    tensor_t aTensor;
    size_t dims[] = {numberOfValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    shape_t shape = {
        .dimensions = dims,
        .numberOfDimensions = numberOfDims,
        .orderOfDimensions = orderOfDims
    };
    setTensorValues(&aTensor, aData, &shape, &aQ, NULL);


    quantization_t bQ;
    initInt32Quantization(&bQ);
    int32_t bData[] = {1, -1, 3, -5, 9, 1};
    tensor_t bTensor;
    setTensorValues(&bTensor, bData, &shape, &bQ, NULL);

    addInt32TensorToSymInt32TensorInplace(&aTensor, &bTensor);

    int32_t expected[] = {2, 1, 6, -1, 14, 7};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aData, numberOfValues);
}

void testAddFloat32TensorToSymInt32TensorInplace() {
    size_t numberOfValues = 6;

    symInt32QConfig_t aQC;
    initSymInt32QConfig(HTE, &aQC);
    aQC.scale = 100.f;
    quantization_t aQ;
    initSymInt32Quantization(&aQC, &aQ);
    int32_t aData[] = {1, 2, 3, 4, 5, 6};
    tensor_t aTensor;
    size_t dims[] = {numberOfValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    shape_t shape = {
        .dimensions = dims,
        .numberOfDimensions = numberOfDims,
        .orderOfDimensions = orderOfDims
    };
    setTensorValues(&aTensor, aData, &shape, &aQ, NULL);


    quantization_t bQ;
    initFloat32Quantization(&bQ);
    float bData[] = {1.1f, -1.1f, -5.9f, 9.3f, 1.9f, -4.4f};
    tensor_t bTensor;
    setTensorValues(&bTensor, bData, &shape, &bQ, NULL);

    addFloat32TensorToSymInt32TensorInplace(&aTensor, &bTensor);

    int32_t expected[] = {2, 1, -2, 13, 6, 2};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, aData, numberOfValues);
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testAddInt32TensorsInplace);
    RUN_TEST(testAddInt32ElementWithInt32TensorInplace);

    RUN_TEST(testAddFloat32TensorsInplace);

    RUN_TEST(testAddSymInt32TensorsInplaceWithSameScale);
    RUN_TEST(testAddSymInt32TensorsInplaceWithDifferentScale);
    RUN_TEST(testAddInt32TensorWithSymInt32TensorInplace);
    RUN_TEST(testAddFloat32TensorToSymInt32TensorInplace);

    UNITY_END();
}