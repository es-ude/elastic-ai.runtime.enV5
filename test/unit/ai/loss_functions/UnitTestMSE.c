#define SOURCE_FILE "MSE-UTEST"

#include "MSE.h"
#include "unity.h"
#include <stdlib.h>

void setUp() {}
void tearDown() {}

void unitTestMSELossDOutput() {
    float output[] = {1.f, 2.f, -3.f};
    float label[] = {-5.f, -4.f, 2.f};
    float *dLossOutput = MSELossDOutput(output, label, sizeof(output) / sizeof(float));
    float expected[] = {6.f, 6.f, -5.f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, dLossOutput, sizeof(output) / sizeof(float));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestMSELossDOutput);
    UNITY_END();
}