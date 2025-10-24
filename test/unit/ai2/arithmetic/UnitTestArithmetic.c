#include "Tensor.h"
#include "unity.h"
#include "Quantization.h"

#include <Add.h>
#include <Arithmetic.h>
#include <DTypes.h>
#include <stdlib.h>
#include <string.h>


void testOrderDims() {
    size_t dimensions[] = {2, 3, 4};
    size_t orderOfDimensions[] = {1, 0, 2};
    size_t numberOfDims = 3;
    tensor_t tensor = {
        .dimensions = dimensions,
        .orderOfDimensions =  orderOfDimensions,
        .numberOfDimensions =  numberOfDims
    };

    size_t expected[] = {3, 2, 4};

    size_t actual[numberOfDims];
    orderDims(&tensor, actual);


    TEST_ASSERT_EQUAL_size_t_ARRAY(expected, actual, numberOfDims);
}


/*23 = [2, 1, 3]

reorderedElementIndex = [1, 2, 3]
dims = 2, 3, 4

size_t index = indices[numDims - 1];               // 3
size_t offset = dims[numDims - 1];                 // 4
for (i = numDims - 2; i >= 0; i--){
    index += indices[i] * offset                   3 + 2 * 4 = 11 | 11 + 1 * 12 = 23
    offset *= dims[i]                              4 * 3 = 12     | 12 * 2 = 24*/
void testCalcTensorIndex() {
    size_t numberOfDimensions = 3;
    size_t dimensions[] = {2, 3, 4};
    size_t indices[] = {1, 2, 3};

    size_t actual = calcTensorIndexByIndices(numberOfDimensions, dimensions, indices);
    size_t expected = 23;
    TEST_ASSERT_EQUAL_size_t(expected, actual);
}

void testCalcIndexByRawIndex() {
    size_t numberOfDimensions = 3;
    size_t dimensions[] = {2, 3, 4};
    size_t expected[] = {1, 2, 3};

    size_t actual[3];
    calcIndicesByRawIndex(numberOfDimensions, dimensions, 23, actual);

    TEST_ASSERT_EQUAL_size_t_ARRAY(expected, actual, numberOfDimensions);
}

void testInt32PointWiseArithmetic() {
    size_t bytesPerElement = sizeof(int32_t);
    size_t numberOfElements = 8;

    uint8_t aData[numberOfElements * bytesPerElement];
    int32_t aValues[] = {-1, 2, 3, 4, 5, 6, -7, 8};
    writeInt32ArrayToByteArray(numberOfElements, aValues, aData);

    size_t aNumberOfDims = 3;
    /*
    [ [ [-1, 2, 3, 4], [5, 6, -7, 8] ] ]


// order: 4, 2, 1

[ [ [-1], [5] ],
  [ [2],  [6] ],
  [ [3], [-7] ],
  [ [4],  [8] ] ]
    */


    size_t aDims[] = {1, 2, 4};

    size_t aOrderDims[] = {0, 1, 2};

    quantization_t aQuantization;
    initInt32Quantization(&aQuantization);


    tensor_t aTensor = {
        .data = aData,
        .dimensions = aDims,
        .numberOfDimensions = aNumberOfDims,
        .orderOfDimensions = aOrderDims,
        .quantization = &aQuantization
    };

    uint8_t bData[numberOfElements * bytesPerElement];
    int32_t bValues[] = {-1, 2, 3, 4, 5, 6, -7, 8};
    writeInt32ArrayToByteArray(numberOfElements, bValues, bData);

    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 1, 4};

    size_t bOrderDims[] = {1, 0, 2};

    quantization_t bQuantization;
    initInt32Quantization(&bQuantization);


    tensor_t bTensor = {
        .data = bData,
        .dimensions = bDims,
        .numberOfDimensions = bNumberOfDims,
        .orderOfDimensions = bOrderDims,
        .quantization = &bQuantization
    };

    uint8_t outputData[numberOfElements * bytesPerElement];
    int32_t outputValues[numberOfElements];

    size_t outputNumberOfDims = 3;
    size_t outputDims[] = {4, 2, 1};

    size_t outputOrderDims[] = {2, 1, 0};

    quantization_t outputQuantization;
    initInt32Quantization(&outputQuantization);


    tensor_t outputTensor = {
        .data = outputData,
        .dimensions = outputDims,
        .numberOfDimensions = outputNumberOfDims,
        .orderOfDimensions = outputOrderDims,
        .quantization = &outputQuantization
    };

    uint8_t expectedData[numberOfElements * bytesPerElement];
    int32_t expectedValues[] = {-2, 10, 4, 12, 6, -14, 8, 16};
    writeInt32ArrayToByteArray(numberOfElements, expectedValues, expectedData);

    size_t expectedNumberOfDims = 3;
    size_t expectedDims[] = {4, 2, 1};

    size_t expectedOrderDims[] = {2, 1, 0};

    quantization_t expectedQuantization;
    initInt32Quantization(&expectedQuantization);


    tensor_t expectedTensor = {
        .data = expectedData,
        .dimensions = expectedDims,
        .numberOfDimensions = expectedNumberOfDims,
        .orderOfDimensions = expectedOrderDims,
        .quantization = &expectedQuantization
    };

    int32PointWiseArithmetic(&aTensor, &bTensor, addInt32s, &outputTensor);
    int32_t actual[numberOfElements];
    readBytesAsInt32Array(numberOfElements, outputTensor.data, actual);
    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedValues, actual, numberOfElements);
}

void setUp(){}
void tearDown(){}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testOrderDims);
    RUN_TEST(testCalcTensorIndex);
    RUN_TEST(testInt32PointWiseArithmetic);
    RUN_TEST(testCalcIndexByRawIndex);
    UNITY_END();
}