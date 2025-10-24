#include "DTypes.h"
#include "Tensor.h"
#include "TensorConversion.h"
#include "unity.h"

#include <stdlib.h>

void testConversionFloatInt32() {
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
    size_t FloatNumBytes = calcBitsPerElement(&floatQ)*num_values;
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
    uint8_t IntSparsityBitmask[num_values/8+1];
    quantization_t intQ={
        .type = INT32,
        .qConfig = NULL
    };
    size_t IntNumBytes = calcBitsPerElement(&intQ)*num_values;
    uint8_t uint8Data[IntNumBytes];
    tensor_t intTensor={
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
}

void testGetBitmask() {
    uint8_t startbit = 1;
    uint8_t endbit = 4;
    uint8_t bitmask = getBitmask(startbit, endbit);
    uint8_t expected = 0b00001110;
    TEST_ASSERT_EQUAL(expected, bitmask);
}
void testGetBitmask2() {
    uint8_t startbit = 10;
    uint8_t endbit = 14;
    uint8_t bitmask = getBitmask(startbit, endbit);
    uint8_t expected = 0b00111100;
    TEST_ASSERT_EQUAL(expected, bitmask);
}

void testReadByte() {
    uint8_t startbit = 1;
    uint8_t endbit = 4;
    uint8_t data = 0b00101010;
    uint8_t actual = readByte(data, startbit, endbit);
    uint8_t expected = 0b00000101;
    TEST_ASSERT_EQUAL(expected, actual);
}

void testWriteByte() {
    uint8_t existing_data = 0b00000101;
    uint8_t data = 0b00000101;
    uint8_t newData = writeByte(existing_data, data, 3, 7);
    uint8_t expected = 0b00101101;
    TEST_ASSERT_EQUAL_UINT8(expected, newData);
}

void testWriteByte2() {
    uint8_t existing_data = 0b00000101;
    uint8_t data = 0b00010101;
    uint8_t newData = writeByte(existing_data, data, 3, 11);
    uint8_t expected = 0b10101101;
    TEST_ASSERT_EQUAL_UINT8(expected, newData);
}

void testByteFlattening(){
    // {1, 2, 78}
    uint8_t dataIn[] = {0b000000001, 0b00000110, 0b00111000, 0b00000001};
    size_t dataInBits = 9;

    size_t dataOutBits = 19;
    size_t numValues = 3;
    size_t numBytesDataOut = (dataOutBits*numValues-1)/8+1;
    uint8_t *dataOut = calloc(numBytesDataOut, sizeof(uint8_t));
    byteConversion(dataIn, dataInBits, dataOut, dataOutBits, numValues);

    uint8_t expectedBytes[] = {0b000000001, 0b00000000, 0b00011000, 0b00000000, 0b10000000, 0b00010011, 0b00000000, 0b00000000};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, dataOut, numBytesDataOut);
}

void testByteFlattening2(){
    // {1, 2, 78}
    uint8_t dataIn[] = {0b000000001, 0b00000000, 0b00011000, 0b00000000, 0b10000000, 0b00010011, 0b00000000, 0b00000000};
    size_t dataInBits = 19;

    size_t dataOutBits = 9;
    size_t numValues = 3;
    size_t numBytesDataOut = (dataOutBits*numValues-1)/8+1;
    uint8_t *dataOut = calloc(numBytesDataOut, sizeof(uint8_t));
    byteConversion(dataIn, dataInBits, dataOut, dataOutBits, numValues);

    uint8_t expectedBytes[] = {0b000000001, 0b00000110, 0b00111000, 0b00000001};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, dataOut, numBytesDataOut);
}

void testByteFlattening3(){
    // {1, 2, 78}
    uint8_t dataIn[] = {0b000000001, 0b00000000, 0b00001100, 0b00000000, 0b00000100, 0b00001011, 0b00000000, 0b00000000};
    size_t dataInBits = 8;

    size_t dataOutBits = 4;
    size_t numValues = 8;
    size_t numBytesDataOut = (dataOutBits*numValues-1)/8+1;
    uint8_t *dataOut = calloc(numBytesDataOut, sizeof(uint8_t));
    byteConversion(dataIn, dataInBits, dataOut, dataOutBits, numValues);

    uint8_t expectedBytes[] = {0b000000001, 0b00001100, 0b10110100, 0b00000000};

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, dataOut, numBytesDataOut);
}

void testByteFlattening4() {
    uint8_t dataIn[] = {0b11010000, 0b11101110, 0b01101111, 0b00000000};
    size_t dataInBits = 5;
    size_t dataOutBits = 8;
    size_t numBytesDataOut = 6;
    uint8_t *dataOut = calloc(numBytesDataOut, sizeof(uint8_t));
    byteConversion(dataIn, dataInBits, dataOut, dataOutBits, numBytesDataOut);
    uint8_t expectedBytes[] = {16, 22, 27, 31, 6, 0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, dataOut, numBytesDataOut);
}

void testByteFlattening5() {
    uint8_t dataIn[] = {0b11010000, 0b11101110, 0b01101111, 0b00000000};
    size_t dataInBits = 5;
    size_t dataOutBits = 32;
    size_t numValues = 6;
    size_t numBytesDataOut = 4*numValues;
    uint8_t dataOut[numBytesDataOut];;
    byteConversion(dataIn, dataInBits, dataOut, dataOutBits, numValues);
    uint8_t expectedBytes[] = {16, 0,0,0,22, 0,0,0,27, 0,0,0,31, 0,0,0,6, 0,0,0,0,0,0,0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBytes, dataOut, numBytesDataOut);

}

void testInitTensor() {}

void testConversionFloatInt() {
    float floatDataF[] = {1., 2., 3., 4., -1., -2.};
    size_t dimensions[] = {6};
    uint8_t numValues = 6;
    quantization_t floatQ = {.type = FLOAT32, .qConfig = NULL};
    uint8_t floatData[numValues*sizeof(float)];
    writeFloatArrayToByteArray(numValues, floatDataF, floatData);
    tensor_t floatT = {
        .data = floatData,
        .quantization = &floatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };

    int32_t intDataExpected[] = {1, 2, 3, 4, -1, -2};

    quantization_t intQ = {.type = INT32, .qConfig = NULL};
    uint8_t intData[numValues*sizeof(int32_t)];
    tensor_t intT = {
        .data = intData,
        .quantization = &intQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };
    convertTensor(&floatT, &intT);

    int32_t actual[numValues];
    readBytesAsInt32Array(6, intData, actual);

    TEST_ASSERT_EQUAL_INT32_ARRAY(intDataExpected, actual, numValues);
}

void testConversionIntFloat() {
    size_t dimensions[] = {6};
    uint8_t numValues = 6;

    int32_t intDataI[] = {1, 2, 3, 4, -1, -2};
    quantization_t intQ = {.type = INT32, .qConfig = NULL};
    uint8_t intData[numValues*sizeof(int32_t)];
    writeInt32ArrayToByteArray(numValues, intDataI, intData);
    tensor_t intT = {
        .data = intData,
        .quantization = &intQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };

    float floatDataFExpected[] = {1.f, 2.f, 3.f, 4.f, -1.f, -2.f};

    quantization_t floatQ = {.type = FLOAT32, .qConfig = NULL};
    uint8_t floatData[numValues*sizeof(float)];
    tensor_t floatT = {
        .data = floatData,
        .quantization = &floatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };
    convertTensor( &intT, &floatT);
    float actual[numValues];
    readBytesAsFloatArray(numValues, floatData, actual);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(floatDataFExpected, actual, numValues);

}

void testConversionIntLinear() {
    size_t numValues = 6;
    size_t dimensions[] = {numValues};

    int32_t intDataI[] = {1, 2, 3, 4, -1, -2};

    quantization_t intQ = {.type = INT32, .qConfig = NULL};
    uint8_t intData[numValues*sizeof(int32_t)];
    writeInt32ArrayToByteArray(numValues, intDataI, intData);
    tensor_t intT = {
        .data = intData,
        .quantization = &intQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };


    linearQConfig_t linQC= {
        .qBits = 5,
        .roundingMode = HTE,
        .scale = 0.f,
        .zeroPoint = 0,
    };
    uint8_t linData[(linQC.qBits*numValues-1)/8+1];
    quantization_t linearQ = {.type = LINEAR, .qConfig = &linQC};
    tensor_t linT = {
        .data = linData,
        .quantization = &linearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };
    convertTensor( &intT, &linT);

    uint8_t flattenedLinData[numValues];
    byteConversion(linT.data, linQC.qBits, flattenedLinData, 8, numValues);

    uint8_t expectedLin[] = {16, 22, 27, 31, 6, 0};
    int32_t expectedZeroPoint = -11;
    float expectedScale = 0.1875f;

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedLin, flattenedLinData, numValues);
    TEST_ASSERT_EQUAL_INT32(expectedZeroPoint, linQC.zeroPoint);
    TEST_ASSERT_EQUAL_FLOAT(expectedScale, linQC.scale);
}

void testConversionLinearInt() {
    size_t numValues = 6;
    size_t dimensions[] = {numValues};

    linearQConfig_t linQC= {
        .qBits = 5,
        .roundingMode = HTE,
        .scale = 0.1875f,
        .zeroPoint = -11,
    };
    uint8_t linData[]= {0b11010000, 0b11101110, 0b01101111, 0b00000000};
    quantization_t linearQ = {.type = LINEAR, .qConfig = &linQC};
    tensor_t linT = {
        .data = linData,
        .quantization = &linearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };

    quantization_t intQ = {.type = INT32, .qConfig = NULL};
    uint8_t intData[numValues*sizeof(int32_t)];
    tensor_t intT = {
        .data = intData,
        .quantization = &intQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };
    convertTensor( &linT, &intT);
    int32_t actual[numValues];
    readBytesAsInt32Array(numValues, intT.data, actual);
    int32_t expectedData[] = {5, 11, 16, 20, -5, -11};
    TEST_ASSERT_EQUAL_INT32_ARRAY(expectedData, actual, numValues);
}

void testConversionFloatLinear() {
    size_t numValues = 6;
    size_t dimensions[] = {numValues};

    float floatDataf[] = {1.f, 2.f, 3.f, 4.f, -1.f, -2.f};

    quantization_t floatQ = {.type = FLOAT32, .qConfig = NULL};
    uint8_t floatData[numValues*sizeof(float)];
    writeFloatArrayToByteArray(numValues, floatDataf, floatData);
    tensor_t floatT = {
        .data = floatData,
        .quantization = &floatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };


    linearQConfig_t linQC= {
        .qBits = 5,
        .roundingMode = HTE,
        .scale = 0.f,
        .zeroPoint = 0,
    };
    uint8_t linData[(linQC.qBits*numValues-1)/8+1];
    quantization_t linearQ = {.type = LINEAR, .qConfig = &linQC};
    tensor_t linT = {
        .data = linData,
        .quantization = &linearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };
    convertTensor( &floatT, &linT);

    uint8_t flattenedLinData[numValues];
    byteConversion(linT.data, linQC.qBits, flattenedLinData, 8, numValues);

    uint8_t expectedLin[] = {16, 22, 27, 31, 6, 0};
    int32_t expectedZeroPoint = -11;
    float expectedScale = 0.1875f;

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedLin, flattenedLinData, numValues);
    TEST_ASSERT_EQUAL_INT32(expectedZeroPoint, linQC.zeroPoint);
    TEST_ASSERT_EQUAL_FLOAT(expectedScale, linQC.scale);
}

void testConversionLinearFloat() {
    size_t numValues = 6;
    size_t dimensions[] = {numValues};

    linearQConfig_t linQC= {
        .qBits = 5,
        .roundingMode = HTE,
        .scale = 0.1875f,
        .zeroPoint = -11,
    };
    uint8_t linData[]= {0b11010000, 0b11101110, 0b01101111, 0b00000000};
    quantization_t linearQ = {.type = LINEAR, .qConfig = &linQC};
    tensor_t linT = {
        .data = linData,
        .quantization = &linearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };

    quantization_t floatQ = {.type = FLOAT32, .qConfig = NULL};
    uint8_t floatData[numValues*sizeof(float)];
    tensor_t floatT = {
        .data = floatData,
        .quantization = &floatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = 1,
        .dimensions = dimensions,
        .orderOfDimensions = NULL
    };
    convertTensor( &linT, &floatT);
    float actual[numValues];
    readBytesAsFloatArray(numValues, floatT.data, actual);
    float expectedData[] = {0.9375f, 2.0625f, 3.f, 3.75f, -0.9375f, -2.0625f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedData, actual, numValues);
}

void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testByteFlattening);
    RUN_TEST(testByteFlattening2);
    RUN_TEST(testByteFlattening3);
    RUN_TEST(testByteFlattening4);
    RUN_TEST(testByteFlattening5);

    RUN_TEST(testGetBitmask);
    RUN_TEST(testGetBitmask2);
    RUN_TEST(testWriteByte);
    RUN_TEST(testWriteByte2);
    RUN_TEST(testReadByte);

    //RUN_TEST(testInitTensor);
    RUN_TEST(testConversionFloatInt);
    RUN_TEST(testConversionIntFloat);
    RUN_TEST(testConversionIntLinear);
    RUN_TEST(testConversionLinearInt);
    RUN_TEST(testConversionFloatLinear);
    RUN_TEST(testConversionLinearFloat);
    UNITY_END();
}