#include "Tensor.h"
#include "unity.h"
#include "Quantization.h"
#include "Arithmetic.h"
#include "Add.h"
#include "Mul.h"
#include "DTypes.h"

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

    /*
    [ [ [-1, 2, 3, 4], [5, 6, -7, 8] ] ]


// order: 4, 2, 1

[ [ [-1], [5] ],
  [ [2],  [6] ],
  [ [3], [-7] ],
  [ [4],  [8] ] ]
    */
    int32_t aData[] = {-1, 2, 3, 4, 5, 6, -7, 8};

    size_t aDims[] = {1, 2, 4};
    size_t aOrderDims[] = {0, 1, 2};
    size_t aNumberOfDims = 3;

    quantization_t aQuantization;
    initInt32Quantization(&aQuantization);

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, aDims, aNumberOfDims, aOrderDims, &aQuantization, NULL);

    int32_t bData[] = {-1, 2, 3, 4, 5, 6, -7, 8};

    size_t bNumberOfDims = 3;
    size_t bDims[] = {2, 1, 4};
    size_t bOrderDims[] = {1, 0, 2};

    quantization_t bQuantization;
    initInt32Quantization(&bQuantization);

    tensor_t bTensor;
    setTensorValues(&bTensor, bData, bDims, bNumberOfDims, bOrderDims, &bQuantization, NULL);

    uint32_t outputData[numberOfElements];

    size_t outputNumberOfDims = 3;
    size_t outputDims[] = {4, 2, 1};
    size_t outputOrderDims[] = {2, 1, 0};

    quantization_t outputQuantization;
    initInt32Quantization(&outputQuantization);

    tensor_t outputTensor;
    setTensorValues(&outputTensor, outputData, outputDims, outputNumberOfDims, outputOrderDims, &outputQuantization, NULL);

    int32_t expectedValues[] = {-2, 4, 6, 8, 10, 12, -14, 16};


    int32PointWiseArithmetic(&aTensor, &bTensor, addInt32s, &outputTensor);

    int32_t actual[numberOfElements];
    readBytesAsInt32Array(numberOfElements, outputTensor.data, actual);

    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedValues, actual, numberOfElements);
}

void testFloat32ElementWithTensorArithmetic() {
    float x = 2.f;

    quantization_t aQ;
    initFloat32Quantization(&aQ);
    float aData[] = {1.f, 2.f, 3.f, 4.f};
    size_t aDims[] = {4};
    size_t aNumberOfDims = 1;
    size_t aOrderOfDims[] = {0};

    tensor_t aTensor;
    setTensorValues(&aTensor, aData, aDims, aNumberOfDims, aOrderOfDims, &aQ, NULL);

    floatElementWithTensorArithmeticInplace(&aTensor, x, mulFloat32s);

    float expected[] = {2.f, 4.f, 6.f, 8.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, aData, 4);

}

void setUp(){}
void tearDown(){}
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testOrderDims);
    RUN_TEST(testCalcTensorIndex);
    RUN_TEST(testCalcIndexByRawIndex);
    RUN_TEST(testInt32PointWiseArithmetic);
    RUN_TEST(testFloat32ElementWithTensorArithmetic);
    UNITY_END();
}