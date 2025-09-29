#define SOURCE_FILE "Softmax-UTEST"

#include "Softmax.h"
#include "unity.h"
#include <stdlib.h>

void unitTestInitSoftmaxConfig() {
    size_t size = 3;
    softmaxConfig_t *config = initSoftmaxConfig(size);

    TEST_ASSERT_EQUAL_size_t(size, config->size);
}

void unitTestInitSoftmaxLayerForward() {
    size_t size = 10;
    layerForward_t *layer = initSoftmaxLayerForward(size);
    softmaxConfig_t *config = layer->config;
    TEST_ASSERT_EQUAL_size_t(size, config->size);
    TEST_ASSERT_EQUAL_PTR(softmaxForward, layerFunctions[layer->type].forwardFunc);
    layerType_t layerType = SOFTMAX;
    TEST_ASSERT_EQUAL(layerType, layer->type);
}

void unitTestInitSoftmaxLayerForwardBackward() {
    size_t size = 10;
    layerForwardBackward_t *layer = initSoftmaxLayerForwardBackward(size);
    softmaxConfig_t *config = layer->config;
    TEST_ASSERT_EQUAL_size_t(size, config->size);
    TEST_ASSERT_EQUAL_PTR(softmaxForward, layerFunctions[layer->type].forwardFunc);
    TEST_ASSERT_EQUAL_PTR(softmaxBackward, layerFunctions[layer->type].backwardFunc);
    layerType_t layerType = SOFTMAX;
    TEST_ASSERT_EQUAL(layerType, layer->type);
}

void unitTestSoftmaxForward() {
    size_t inputSize = 6;
    float input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};

    softmaxConfig_t *config = initSoftmaxConfig(sizeof(input) / sizeof(float));

    float *result = softmaxForward(config, input, inputSize);
    float expected[] = {2.3008e-03, 6.2543e-03, 1.7001e-02, 4.6213e-02, 9.2822e-01, 1.5503e-05};

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], result[i]);
    }
}

void unitTestSoftmaxBackward() {
    float input[] = {2.3008e-03, 6.2543e-03, 1.7001e-02, 4.6213e-02, 9.2822e-01, 1.5503e-05};
    float grad[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};

    softmaxConfig_t config;
    config.size = sizeof(input) / sizeof(float);

    float *result = softmaxBackward(&config, grad, input, 6);
    float expected[] = {-6.9173e-03, -6.2947e-03, -1.1912e-01, 1.3834e-01, -5.9973e-03,
                        -1.5603e-05};

    for (size_t i = 0; i < config.size; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], result[i]);
    }
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitSoftmaxConfig);
    RUN_TEST(unitTestInitSoftmaxLayerForward);
    RUN_TEST(unitTestInitSoftmaxLayerForwardBackward);
    RUN_TEST(unitTestSoftmaxForward);
    RUN_TEST(unitTestSoftmaxBackward);
    UNITY_END();
}
