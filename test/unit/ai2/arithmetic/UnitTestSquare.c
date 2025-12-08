#include "Square.h"
#include "unity.h"

void setUp(){}
void tearDown(){}

void testSquareInt32() {
    int32_t a = 3;
    int32_t actual = squareInt32(a);
    int32_t expected = 9;

    TEST_ASSERT_EQUAL_INT32(expected, actual);
}

void testLogFloat() {
    float a = 0.5f;
    float actual = squareFloat32(a);
    float expected = 0.25f;

    TEST_ASSERT_EQUAL_FLOAT(expected, actual);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testSquareInt32);
    RUN_TEST(testLogFloat);
    UNITY_END();
}