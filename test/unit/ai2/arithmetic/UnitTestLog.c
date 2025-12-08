#include "Log.h"
#include "unity.h"

void testLogFloat() {
    float a = 4.f;
    float actual = logFloat(a);
    float expected = 1.38629436112f;

    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}

void setUp(){}
void tearDown(){}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testLogFloat);
    UNITY_END();
}