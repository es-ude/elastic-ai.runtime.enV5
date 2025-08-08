#define SOURCE_FILE "RELU-UTEST"

#include <stdlib.h>

#include "ReLU.h"
#include "unity.h"

void unitTestReLUForward() {
    float input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};

    ReLUConfig_t *config = initReLUConfig(sizeof(input) / sizeof(float));

    float *result = ReLUForward(config, input);
    float expected[] = {0.f, 0.f, 1.f, 2.f, 5.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, result, config->size);
}

void unitTestReLUBackward() {
    float input[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    float grad[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};

    ReLUConfig_t config;
    config.size = sizeof(input) / sizeof(float);

    float *result = ReLUBackward(&config, grad, input);
    float expected[] = {0.f, 0.f, -4.f, 6.f, 3.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, result, config.size);
}

void unitTestInitReLUConfig() {
    size_t size = 3;
    ReLUConfig_t *config = initReLUConfig(size);

    TEST_ASSERT_EQUAL_size_t(size, config->size);
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestReLUForward);
    RUN_TEST(unitTestReLUBackward);
    RUN_TEST(unitTestInitReLUConfig);
    UNITY_END();
}
