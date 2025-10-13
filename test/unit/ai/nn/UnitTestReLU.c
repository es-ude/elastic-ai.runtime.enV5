#define SOURCE_FILE "RELU-UTEST"

#include <stdlib.h>

#include "ReLU.h"
#include "unity.h"

#include <string.h>

void unitTestReLUForwardFloat32() {
    quantization_t *inputQ = initQuantizationByType(FLOAT32_Q);
    float data[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t numberOfDims = 1;
    size_t dims[] = {6};
    qTensor_t *inputQTensor = initQTensor(data, numberOfDims, dims, inputQ);

    quantization_t *outputQ = initQuantizationByType(FLOAT32_Q);
    ReLUConfig_t *config = initReLUConfig(0);
    qTensor_t *resultQTensor = ReLUForward(config, inputQTensor, outputQ);

    float expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};

    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t bytesPerElement = calcBytesPerElement(inputQ);

    for (size_t i = 0; i < totalNumberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        float32_t currentElement = readBytesAsFloat32(&resultQTensor->data[byteIndex]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], currentElement);
    }
}

void unitTestReLUForwardFloat64() {
    quantization_t *inputQ = initQuantizationByType(FLOAT64_Q);

    double data[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t numberOfDims = 1;
    size_t dims[] = {6};
    qTensor_t *inputQTensor = initQTensor(data, numberOfDims, dims, inputQ);

    quantization_t *outputQ = initQuantizationByType(FLOAT64_Q);
    ReLUConfig_t *config = initReLUConfig(0);
    qTensor_t *resultQTensor = ReLUForward(config, inputQTensor, outputQ);

    double expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};
    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t bytesPerElement = calcBytesPerElement(inputQ);
    uint8_t *expectedRaw = calloc(totalNumberOfElements, bytesPerElement);
    memcpy(expectedRaw, expected, totalNumberOfElements * bytesPerElement);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedRaw, resultQTensor->data,
                                  totalNumberOfElements*bytesPerElement);
}

void unitTestReLUBackwardFloat32() {
    quantization_t *inputQ = initQuantizationByType(FLOAT32_Q);
    float32_t input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputNumberOfDims = 1;
    size_t inputDims[] = {6};
    qTensor_t *inputQTensor = initQTensor(input, inputNumberOfDims, inputDims, inputQ);

    quantization_t *gradQ = initQuantizationByType(FLOAT32_Q);
    float32_t grad[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    size_t gradNumberOfDims = 1;
    size_t gradDims[] = {6};
    qTensor_t *gradQTensor = initQTensor(grad, gradNumberOfDims, gradDims, gradQ);

    ReLUConfig_t *config = initReLUConfig();

    qTensor_t *actualQTensor = ReLUBackward(config, gradQTensor, inputQTensor);

    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    /*size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t bytesPerElement = calcBytesPerElement(inputQ);
    float32_t *actual = calloc(totalNumberOfElements, bytesPerElement);
    memcpy(actual, actualQTensor->data, totalNumberOfElements * bytesPerElement);

    for(size_t i = 0; i < totalNumberOfElements; i++) {
        printf("output: %f\n", actual[i]);
    }*/

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actualQTensor->data, 6);
}

void unitTestReLUBackwardFloat64() {
    float64_t input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputNumberOfDims = 1;
    size_t inputDims[] = {6};
    quantization_t *inputQ = initQuantizationByType(FLOAT64_Q);
    qTensor_t *inputQTensor = initQTensor(input, inputNumberOfDims, inputDims, inputQ);

    float64_t grad[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    size_t gradNumberOfDims = 1;
    size_t gradDims[] = {6};
    quantization_t *gradQ = initQuantizationByType(FLOAT64_Q);
    qTensor_t *gradQTensor = initQTensor(grad, gradNumberOfDims, gradDims, gradQ);

    ReLUConfig_t *config = initReLUConfig();

    qTensor_t *actualQTensor = ReLUBackward(config, gradQTensor, inputQTensor);

    double expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};
    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t bytesPerElement = calcBytesPerElement(inputQ);
    uint8_t *expectedRaw = calloc(totalNumberOfElements, bytesPerElement);
    memcpy(expectedRaw, expected, totalNumberOfElements * bytesPerElement);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedRaw, actualQTensor->data,
                                  totalNumberOfElements*bytesPerElement);
}


void unitTestInitReLULayerForward() {
    layerForward_t *layer = initReLULayerForward();
    TEST_ASSERT_EQUAL_PTR(ReLUForward, layerFunctions[layer->type].forwardFunc);
    layerType_t layerType = RELU;
    TEST_ASSERT_EQUAL(layerType, layer->type);
}

void unitTestInitReLULayerBackward() {
    layerForwardBackward_t *layer = initReLULayerForwardBackward();
    TEST_ASSERT_EQUAL_PTR(ReLUForward, layerFunctions[layer->type].forwardFunc);
    TEST_ASSERT_EQUAL_PTR(ReLUBackward, layerFunctions[layer->type].backwardFunc);
    layerType_t layerType = RELU;
    TEST_ASSERT_EQUAL(layerType, layer->type);
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestReLUForwardFloat32);
    RUN_TEST(unitTestReLUForwardFloat64);
    RUN_TEST(unitTestReLUBackwardFloat32);
    RUN_TEST(unitTestReLUBackwardFloat64);
    RUN_TEST(unitTestInitReLULayerForward);
    RUN_TEST(unitTestInitReLULayerBackward);
    UNITY_END();
}
