#define SOURCE_FILE "MSE-UTEST"

#include "MSE.h"
#include "unity.h"
#include <stdlib.h>

void setUp() {}
void tearDown() {}

void unitTestMSELossDOutput() {
    float output[] = {1.f, 2.f, -3.f};
    float label[] = {-5.f, -4.f, 2.f};
    float *actual = MSELossBackward(output, label, sizeof(output) / sizeof(float));
    float expected[] = {4.f, 4.f, -3.3333f};

    for(size_t i = 0; i < 3; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual[i]);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestMSELossDOutput);
    return UNITY_END();
}