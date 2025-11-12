#include "DTypes.h"
#include "Quantization.h"
#include "Tensor.h"
#include "TensorConversion.h"
#include "unity.h"

#include <stddef.h>
#include <stdlib.h>

/*void testConversionFloatInt32() {
    float expected[] = {1, 2, 3};

    size_t num_values = 3;
    float floatData[] = {1.f, 2.f, 3.f};
    size_t floatDims[] = {3};
    size_t floatOrders[] = {0};
    uint8_t floatSparsityBitmask[] = {0x03};
    quantization_t floatQ = {
        .type = FLOAT32,
        .qConfig = NULL
    };
    size_t FloatNumBytes = calcBitsPerElement(&floatQ) * num_values;
    uint8_t floatUInt8Data[FloatNumBytes];
    writeFloatArrayToByteArray(num_values, floatData, floatUInt8Data);
    tensor_t floatTensor = {
        .data = floatUInt8Data,
        .quantization = &floatQ,
        .sparsityBitmask = floatSparsityBitmask,
        .numberOfDimensions = 1,
        .dimensions = floatDims,
        .orderOfDimensions = floatOrders
    };

    int32_t intData[3];
    size_t intDims[1];
    size_t intOrders[1];
    uint8_t IntSparsityBitmask[num_values / 8 + 1];
    quantization_t intQ = {
        .type = INT32,
        .qConfig = NULL
    };
    size_t IntNumBytes = calcBitsPerElement(&intQ) * num_values;
    uint8_t uint8Data[IntNumBytes];
    tensor_t intTensor = {
        .data = uint8Data,
        .quantization = &intQ,
        .sparsityBitmask = IntSparsityBitmask,
        .numberOfDimensions = 1,
        .dimensions = intDims,
        .orderOfDimensions = intOrders
    };

    convertTensor(&floatTensor, &intTensor);
    readBytesAsInt32Array(num_values, uint8Data, intData);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, intData, num_values);
}*/


void testConversionIntFloat() {
    uint8_t numValues = 6;

    size_t dims[] = {6};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    int32_t intData[] = {1, 2, 3, 4, -1, -2};
    quantization_t intQ;
    initInt32Quantization(&intQ);
    tensor_t intTensor;
    setTensorValues(&intTensor, intData, dims, numberOfDims, orderOfDims, &intQ, NULL);

    quantization_t floatQ;
    initFloat32Quantization(&floatQ);
    float floatData[numValues];

    tensor_t floatTensor;
    setTensorValues(&floatTensor, floatData, dims, numberOfDims, orderOfDims, &floatQ, NULL);

    convertTensor(&intTensor, &floatTensor);
    float actual[numValues];
    readBytesAsFloatArray(numValues, floatData, actual);

    float expected[] = {1.f, 2.f, 3.f, 4.f, -1.f, -2.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, numValues);

}
void testConversionIntSymInt32() {
    uint8_t numValues = 6;

    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    int32_t intData[] = {1, 2, 3, 4, -1, -2};

    quantization_t intQ;
    initInt32Quantization(&intQ);
    tensor_t intTensor;
    setTensorValues(&intTensor, intData, dims, numberOfDims, orderOfDims, &intQ, NULL);

    symInt32QConfig_t symInt32QConfig;
    initSymInt32QConfig(HTE, &symInt32QConfig);
    quantization_t symInt32Q;
    initSymInt32Quantization(&symInt32QConfig, &symInt32Q);

    int32_t symInt32Data[numValues];

    tensor_t symInt32Tensor;
    setTensorValues(&symInt32Tensor, symInt32Data, dims, numberOfDims, orderOfDims, &symInt32Q, NULL);

    convertTensor(&intTensor, &symInt32Tensor);

    TEST_ASSERT_EQUAL_INT32_ARRAY(intTensor.data, symInt32Tensor.data, numValues);

}
void testConversionIntAsym() {
    size_t numValues = 6;
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};
    int32_t intData[] = {1, 2, 3, 4, -1, -2};

    quantization_t intQ;
    initInt32Quantization(&intQ);

    tensor_t intTensor;
    setTensorValues(&intTensor, intData, dims, numberOfDims, orderOfDims, &intQ, NULL);

    asymQConfig_t asymQConfig;
    initAsymQConfig(5, HTE, &asymQConfig);
    quantization_t asymQ;
    initAsymQuantization(&asymQConfig, &asymQ);
    uint8_t asymData[numValues * calcBytesPerElement(&asymQ)];

    tensor_t asymTensor;
    setTensorValues(&asymTensor, asymData, dims, numberOfDims, orderOfDims, &asymQ, NULL);
    convertTensor(&intTensor, &asymTensor);

    uint8_t flattenedAsymData[numValues];
    byteConversion(asymTensor.data, asymQConfig.qBits, flattenedAsymData, 8, numValues);

    /*uint8_t expectedAsym[] = {16, 22, 25, 31, 6, 0};
    int32_t expectedZeroPoint = -11;
    float expectedScale = 0.1875f;*/

    uint8_t expectedAsym[] = {15, 20, 26, 30, 5, 0};
    int32_t expectedZeroPoint = -10;
    float expectedScale = 0.1935484f;

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedAsym, flattenedAsymData, numValues);
    TEST_ASSERT_EQUAL_INT32(expectedZeroPoint, asymQConfig.zeroPoint);
    TEST_ASSERT_EQUAL_FLOAT(expectedScale, asymQConfig.scale);
}


void testConversionFloatInt() {
    uint8_t numValues = 6;

    float floatData[] = {1.f, 2.f, 3.f, 4.f, -1.f, -2.f};
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    quantization_t floatQ;
    initFloat32Quantization(&floatQ);

    tensor_t floatTensor;
    setTensorValues(&floatTensor, floatData, dims, numberOfDims, orderOfDims, &floatQ, NULL);

    quantization_t intQ;
    initInt32Quantization(&intQ);
    int32_t intData[numValues];
    tensor_t intTensor;
    setTensorValues(&intTensor, intData, dims, numberOfDims, orderOfDims, &intQ, NULL);
    convertTensor(&floatTensor, &intTensor);

    int32_t actual[numValues];
    readBytesAsInt32Array(6, intData, actual);

    int32_t expected[] = {1, 2, 3, 4, -1, -2};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, numValues);
}
void testConversionFloatSymInt32() {
    uint8_t numValues = 6;

    float floatData[] = {1.1f, 2.1f, 3.1f, 4.1f, -1.1f, -2.1f};
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    quantization_t floatQ;
    initFloat32Quantization(&floatQ);

    tensor_t floatTensor;
    setTensorValues(&floatTensor, floatData, dims, numberOfDims, orderOfDims, &floatQ, NULL);

    symInt32QConfig_t symInt32QConfig;
    initSymInt32QConfig(HTE, &symInt32QConfig);
    quantization_t symInt32Q;
    initSymInt32Quantization(&symInt32QConfig, &symInt32Q);

    int32_t symInt32Data[numValues];
    tensor_t symInt32Tensor;
    setTensorValues(&symInt32Tensor, symInt32Data, dims, numberOfDims, orderOfDims, &symInt32Q, NULL);
    convertTensor(&floatTensor, &symInt32Tensor);

    int32_t expected[] = {1, 2, 3, 4, -1, -2};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, symInt32Tensor.data, numValues);
}
void testConversionFloatAsym() {
    size_t numValues = 6;
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    float floatData[] = {1.f, 2.f, 3.f, 4.f, -1.f, -2.f};

    quantization_t floatQ;
    initFloat32Quantization(&floatQ);
    tensor_t floatTensor;
    setTensorValues(&floatTensor, floatData, dims, numberOfDims, orderOfDims, &floatQ, NULL);

    asymQConfig_t asymQConfig;
    initAsymQConfig(5, HTE, &asymQConfig);
    quantization_t asymQ;
    initAsymQuantization(&asymQConfig, &asymQ);

    uint8_t asymData[numValues * calcBytesPerElement(&asymQ)];

    tensor_t asymTensor;
    setTensorValues(&asymTensor, asymData, dims, numberOfDims, orderOfDims, &asymQ, NULL);

    convertTensor(&floatTensor, &asymTensor);

    uint8_t flattenedAsymData[numValues];
    byteConversion(asymTensor.data, asymQConfig.qBits, flattenedAsymData, 8, numValues);

    uint8_t expectedAsym[] = {16, 22, 27, 31, 6, 0};
    int32_t expectedZeroPoint = -11;
    float expectedScale = 0.1875f;

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedAsym, flattenedAsymData, numValues);
    TEST_ASSERT_EQUAL_INT32(expectedZeroPoint, asymQConfig.zeroPoint);
    TEST_ASSERT_EQUAL_FLOAT(expectedScale, asymQConfig.scale);
}

void testConversionSymInt32Int() {
    size_t numValues = 6;

    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    symInt32QConfig_t symInt32QConfig;
    initSymInt32QConfig(HTE, &symInt32QConfig);
    quantization_t symInt32Q;
    initSymInt32Quantization(&symInt32QConfig, &symInt32Q);

    int32_t symInt32Data[] = {1, 2, 3, 4, -1, -2};
    tensor_t symInt32Tensor;
    setTensorValues(&symInt32Tensor, symInt32Data, dims, numberOfDims, orderOfDims, &symInt32Q, NULL);


    int32_t intData[numValues];
    quantization_t intQ;
    initInt32Quantization(&intQ);
    tensor_t intTensor;
    setTensorValues(&intTensor, intData, dims, numberOfDims, orderOfDims, &intQ, NULL);

    convertTensor(&symInt32Tensor, &intTensor);

    int32_t expected[] = {1, 2, 3, 4, -1, -2};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected, intTensor.data, numValues);
}
void testConversionSymInt32Float() {
    uint8_t numValues = 6;

    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    symInt32QConfig_t symInt32QConfig;
    initSymInt32QConfig(HTE, &symInt32QConfig);
    symInt32QConfig.scale = 1.f;
    quantization_t symInt32Q;
    initSymInt32Quantization(&symInt32QConfig, &symInt32Q);

    int32_t symInt32Data[] = {1, 2, 3, 4, -1, -2};
    tensor_t symInt32Tensor;
    setTensorValues(&symInt32Tensor, symInt32Data, dims, numberOfDims, orderOfDims, &symInt32Q, NULL);

    quantization_t floatQ;
    initFloat32Quantization(&floatQ);

    float floatData[numValues];

    tensor_t floatTensor;
    setTensorValues(&floatTensor, floatData, dims, numberOfDims, orderOfDims, &floatQ, NULL);

    convertTensor(&symInt32Tensor, &floatTensor);

    float expected[] = {1.f, 2.f, 3.f, 4.f, -1.f, -2.f};

    /*float actual[numValues];
    readBytesAsFloatArray(numValues, floatTensor.data, actual);
    for(size_t i = 0; i < numValues; i++) {
        printf("%f\n", actual[i]);
    }*/

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, floatTensor.data, numValues);
}
void testConversionSymInt32Asym() {
    uint8_t numValues = 6;

    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    symInt32QConfig_t symInt32QConfig;
    initSymInt32QConfig(HTE, &symInt32QConfig);
    symInt32QConfig.scale = 1.f;
    quantization_t symInt32Q;
    initSymInt32Quantization(&symInt32QConfig, &symInt32Q);

    int32_t symInt32Data[] = {1, 2, 3, 4, -1, -2};
    tensor_t symInt32Tensor;
    setTensorValues(&symInt32Tensor, symInt32Data, dims, numberOfDims, orderOfDims, &symInt32Q, NULL);

    asymQConfig_t asymQConfig;
    initAsymQConfig(5, HTE, &asymQConfig);
    quantization_t asymQ;
    initAsymQuantization(&asymQConfig, &asymQ);

    size_t outputBitsPerElement = calcBitsPerElement(&asymQ);
    size_t outputTotalNumberOfBits = outputBitsPerElement * numValues;
    size_t numberOfRequiredBytes = ceil((double)outputTotalNumberOfBits / (double)8);
    uint8_t asymData[numberOfRequiredBytes];

    tensor_t asymTensor;
    setTensorValues(&asymTensor, asymData, dims, numberOfDims, orderOfDims, &asymQ, NULL);

    convertTensor(&symInt32Tensor, &asymTensor);

    uint32_t output[numValues];
    byteConversion(asymTensor.data, asymQConfig.qBits, output, 32, numValues);

    float expectedScale = 0.193548f;
    int16_t expectedZeroPoint = -10;
    uint32_t expectedValues[] = {15, 20, 26, 31, 5, 0};

    TEST_ASSERT_EQUAL_FLOAT(expectedScale, asymQConfig.scale);
    TEST_ASSERT_EQUAL_INT16(expectedZeroPoint, asymQConfig.zeroPoint);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(expectedValues, output, numValues);
}

void testConversionAsymInt() {
    size_t numValues = 6;
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    asymQConfig_t asymQConfig;
    initAsymQConfig(5, HTE, &asymQConfig);
    asymQConfig.scale = 0.1875f;
    asymQConfig.zeroPoint = -11;

    quantization_t asymQ;
    initAsymQuantization(&asymQConfig, &asymQ);

    uint8_t asymData[] = {0b11010000, 0b11101110, 0b01101111, 0b00000000};

    tensor_t asymTensor;
    setTensorValues(&asymTensor, asymData, dims, numberOfDims, orderOfDims, &asymQ, NULL);

    quantization_t intQ;
    initInt32Quantization(&intQ);
    int32_t intData[numValues];
    tensor_t intTensor;
    setTensorValues(&intTensor, intData, dims, numberOfDims, orderOfDims, &intQ, NULL);

    convertTensor(&asymTensor, &intTensor);

    int32_t actual[numValues];
    readBytesAsInt32Array(numValues, intTensor.data, actual);
    int32_t expectedData[] = {5, 11, 16, 20, -5, -11};
    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedData, actual, numValues);
}
void testConversionAsymFloat() {
    size_t numValues = 6;
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    asymQConfig_t asymQConfig;
    initAsymQConfig(5, HTE, &asymQConfig);
    asymQConfig.scale = 0.1875f;
    asymQConfig.zeroPoint = -11;
    quantization_t asymQ;
    initAsymQuantization(&asymQConfig, &asymQ);

    uint8_t asymData[] = {0b11010000, 0b11101110, 0b01101111, 0b00000000};

    tensor_t asymTensor;
    setTensorValues(&asymTensor, asymData, dims, numberOfDims, orderOfDims, &asymQ, NULL);

    quantization_t floatQ;
    initFloat32Quantization(&floatQ);
    float floatData[numValues];

    tensor_t floatTensor;
    setTensorValues(&floatTensor, floatData, dims, numberOfDims, orderOfDims, &floatQ, NULL);

    convertTensor(&asymTensor, &floatTensor);

    float actual[numValues];
    readBytesAsFloatArray(numValues, floatTensor.data, actual);
    float expectedData[] = {0.9375f, 2.0625f, 3.f, 3.75f, -0.9375f, -2.0625f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedData, actual, numValues);
}
void testConversionAsymSymInt32() {
    size_t numValues = 6;
    size_t dims[] = {numValues};
    size_t numberOfDims = 1;
    size_t orderOfDims[] = {0};

    asymQConfig_t asymQConfig;
    initAsymQConfig(5, HTE, &asymQConfig);
    asymQConfig.scale = 0.1875f;
    asymQConfig.zeroPoint = -11;
    quantization_t asymQ;
    initAsymQuantization(&asymQConfig, &asymQ);

    uint8_t asymData[] = {0b11010000, 0b11101110, 0b01101111, 0b00000000};

    tensor_t asymTensor;
    setTensorValues(&asymTensor, asymData, dims, numberOfDims, orderOfDims, &asymQ, NULL);

    symInt32QConfig_t symInt32QConfig;
    initSymInt32QConfig(HTE, &symInt32QConfig);
    quantization_t symInt32Q;
    initSymInt32Quantization(&symInt32QConfig, &symInt32Q);
    int32_t symInt32Data[numValues];

    tensor_t symInt32Tensor;
    setTensorValues(&symInt32Tensor, symInt32Data, dims, numberOfDims, orderOfDims, &symInt32Q, NULL);

    convertTensor(&asymTensor, &symInt32Tensor);

    int32_t actual[numValues];
    readBytesAsInt32Array(numValues, symInt32Tensor.data, actual);

    int32_t expectedData[] = {5, 11, 16, 20, -5, -11};

    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedData, actual, numValues);
}


void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();


    RUN_TEST(testConversionIntFloat);
    RUN_TEST(testConversionIntSymInt32);
    RUN_TEST(testConversionIntAsym);


    RUN_TEST(testConversionFloatInt);
    RUN_TEST(testConversionFloatSymInt32);
    RUN_TEST(testConversionFloatAsym);


    RUN_TEST(testConversionSymInt32Int);
    RUN_TEST(testConversionSymInt32Float);
    RUN_TEST(testConversionSymInt32Asym);


    RUN_TEST(testConversionAsymInt);
    RUN_TEST(testConversionAsymFloat);
    RUN_TEST(testConversionAsymSymInt32);

    UNITY_END();
}
