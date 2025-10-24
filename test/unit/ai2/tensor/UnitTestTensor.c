#include "Tensor.h"
#include "unity.h"

void testInitTensor() {}

void testConversionFloatLinear() {
    float expected[] = {1, 2, 3};
    float floatData[] = {1, 2, 3};
    size_t floatDims[] = {3};
    quantization_t *floatQ = initQuantization(FLOAT32);
    tensor_t *floatTensor = initTensor(floatData, floatQ, NULL, 1, floatDims);

    uint8_t linearData[] = {0, 0, 0};
    size_t linearDims[] = {3};
    quantization_t *linearQ = initQuantization(LINEAR);
    tensor_t *linearTensor = initTensor(linearData, linearQ, NULL, 1, linearDims);

    convertTensor(floatTensor, linearTensor);
    convertTensor(linearTensor, floatTensor);

    size_t outputBytesPerElement = calcBytesPerElement(floatQ);

    for (size_t i = 0; i < 3; i++) {
        size_t byteIndex = i * outputBytesPerElement;
        float elem = readBytesAsFloat(&floatTensor->data[byteIndex]);
        TEST_ASSERT_FLOAT_WITHIN(0.01f, expected[i], elem);
    }
}

void testConversionFloatInt32() {
    float expected[] = {1, 2, 3};

    float floatData[] = {1, 2, 3};
    size_t floatDims[] = {3};
    quantization_t *floatQ = initQuantization(FLOAT32);
    tensor_t *floatTensor = initTensor(floatData, floatQ, NULL, 1, floatDims);

    int32_t intData[] = {0, 0, 0};
    size_t intDims[] = {3};
    quantization_t *intQ = initQuantization(INT32);
    tensor_t *intTensor = initTensor(intData, intQ, NULL, 1, intDims);

    convertTensor(floatTensor, intTensor);
    convertTensor(intTensor, floatTensor);

    size_t outputBytesPerElement = calcBytesPerElement(floatQ);


    for (size_t i = 0; i < 3; i++) {
        size_t byteIndex = i * outputBytesPerElement;
        float elem = readBytesAsFloat(&floatTensor->data[byteIndex]);
        TEST_ASSERT_EQUAL(expected[i], elem);
    }
}

void testConversionLinearFloat() {
    uint8_t expected[] = {0, 127, 255};

    uint8_t linearData[] = {1, 2, 3};
    size_t linearDims[] = {3};
    quantization_t *linearQ = initQuantization(LINEAR);
    linearQConfig_t *linearQConfig = linearQ->qConfig;
    linearQConfig->scale = 1;
    linearQConfig->roundingMode = HTE;
    linearQConfig->zeroPoint = 12;
    tensor_t *linearTensor = initTensor(linearData, linearQ, NULL, 1, linearDims);

    float floatData[] = {0, 0, 0};
    size_t floatDims[] = {3};
    quantization_t *floatQ = initQuantization(FLOAT32);
    tensor_t *floatTensor = initTensor(floatData, floatQ, NULL, 1, floatDims);

    convertTensor(linearTensor, floatTensor);
    convertTensor(floatTensor, linearTensor);

    size_t outputBytesPerElement = calcBytesPerElement(linearQ);

    for (size_t i = 0; i < 3; i++) {
        size_t byteIndex = i * outputBytesPerElement;
        uint8_t actual = linearTensor->data[byteIndex];
        TEST_ASSERT_EQUAL(expected[i], actual);
    }
}

void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testInitTensor);
    RUN_TEST(testConversionFloatLinear);
    RUN_TEST(testConversionFloatInt32);
    RUN_TEST(testConversionLinearFloat);
    UNITY_END();
}
