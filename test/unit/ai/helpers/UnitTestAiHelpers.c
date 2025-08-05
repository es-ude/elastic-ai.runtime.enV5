#define SOURCE_FILE "AI-HELPERS-UTEST"

#include <stdlib.h>

#include "AiHelpers.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void unitTestInitParameter() {
    float p[] = {1.f, 2.f, 3.f};
    size_t size = sizeof(p) / sizeof(p[0]);

    parameter_t *parameter = initParameter(p, size);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(p, parameter->p, size);

    float expectedGrad[] = {0.f, 0.f, 0.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedGrad, parameter->grad, size);

    TEST_ASSERT_EQUAL_size_t(size, parameter->size);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitParameter);
    UNITY_END();
}

