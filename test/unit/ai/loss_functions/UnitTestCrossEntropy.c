#define SOURCE_FILE "CrossEntropy-UTEST"

#include "CrossEntropy.h"
#include "unity.h"

void setUp() {}
void tearDown() {}


void unitTestCrossEntropyForward() {
    float logits[] = {2.f, 1.f, 0.1f};
    size_t inputSize = sizeof(logits) / sizeof(logits[0]);
    int target = 0;
    float expected = 0.4170299470424652f;

    float actual = crossEntropyForward(logits, target, inputSize);

    TEST_ASSERT_EQUAL(expected, actual);
}

void unitTestCrossEntropyBackward() {
    float logits[] = {2.f, 1.f, 0.1f};
    size_t inputSize = sizeof(logits) / sizeof(logits[0]);
    int target = 0;
    float t[] = {0.f, 0.f, 0.f};
    t[target] = 1.f;

    float softmaxResults[] = {0.6590f, 0.2424f, 0.0986f};

    float expected[inputSize];
    for (size_t i = 0; i < inputSize; i++) {
        expected[i] = softmaxResults[i] - t[i];
    }

    float *actual = crossEntropyBackward(logits, target, inputSize);

    for(size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_EQUAL(expected[i], actual[i]);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestCrossEntropyForward);
    RUN_TEST(unitTestCrossEntropyBackward);
    return UNITY_END();
}
