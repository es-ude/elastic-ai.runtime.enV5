#define SOURCE_FILE "CrossEntropy-UTEST"

#include "CrossEntropy.h"
#include "unity.h"
#include "../../../../src/ai/nn/include/Softmax.h"

#include <string.h>

void setUp() {}
void tearDown() {}


void unitTestCrossEntropyForwardWithIndex() {
    float logits[] = {2.f, 1.f, 0.1f};
    size_t inputSize = sizeof(logits) / sizeof(logits[0]);
    softmaxConfig_t *config = initSoftmaxConfig(inputSize);
    float *softmaxOutput = softmaxForward(config, logits, inputSize);
    int classIndex = 0;

    float expected = 0.4170299470424652f;

    float actual = crossEntropyForwardWithIndex(softmaxOutput, classIndex);

    TEST_ASSERT_EQUAL_FLOAT(expected, actual);

}

void unitTestCrossEntropyForwardWithDistribution() {
    float logits[] = {2.f, 1.f, 0.1f};
    size_t inputSize = sizeof(logits) / sizeof(logits[0]);
    softmaxConfig_t *config = initSoftmaxConfig(inputSize);
    float *softmaxOutput = softmaxForward(config, logits, inputSize);
    float distribution[] = {0.9f, 0.1f, 0.0f};

    float expected = 0.5170299410820007f;
    float actual = crossEntropyForwardWithDistribution(softmaxOutput, distribution, inputSize);

    TEST_ASSERT_EQUAL_FLOAT(expected, actual);

}

void unitTestCrossEntropyBackwardWithIndex() {
    float logits[] = {2.f, 1.f, 0.1f};
    size_t inputSize = sizeof(logits) / sizeof(logits[0]);
    softmaxConfig_t *config = initSoftmaxConfig(inputSize);
    float *softmaxOutput = softmaxForward(config, logits, inputSize);
    int classIndex = 0;

    float t[] = {0.f, 0.f, 0.f};
    t[classIndex] = 1.f;

    float expected[] = {-0.3410f, 0.2424f, 0.0986f};

    float *actual = crossEntropyBackwardWithIndex(softmaxOutput, classIndex, inputSize);

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual[i]);
    }
}

void unitTestCrossEntropyBackwardWithDistribution() {
    float logits[] = {2.f, 1.f, 0.1f};
    size_t inputSize = sizeof(logits) / sizeof(logits[0]);
    softmaxConfig_t *config = initSoftmaxConfig(inputSize);
    float *softmaxOutput = softmaxForward(config, logits, inputSize);

    float distribution[] = {0.9f, 0.1f, 0.0f};

    float expected[] = {-0.2410f, 0.1424f, 0.0986f};

    float *actual = crossEntropyBackwardWithDistribution(softmaxOutput, distribution, inputSize);

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual[i]);
    }

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestCrossEntropyForwardWithIndex);
    RUN_TEST(unitTestCrossEntropyForwardWithDistribution);
    RUN_TEST(unitTestCrossEntropyBackwardWithIndex);
    RUN_TEST(unitTestCrossEntropyBackwardWithDistribution);
    return UNITY_END();
}
