#include "BinaryArithmeticAbstraction.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void shouldFail(void) {
    TEST_FAIL();
}

void leftShiftShouldSucceed(void) {
    uint32_t actual = leftShift(22, 3);
    uint32_t expected = 176;
    TEST_ASSERT_EQUAL(expected, actual);
}
void rightShiftShouldSucceed(void) {
    uint32_t actual = rightShift(22, 3);
    uint32_t expected = 2;
    TEST_ASSERT_EQUAL(expected, actual);
}
    /*
    0000000000000000000 00010110 22
    00000000000000000000 0000010 right 2
    0000000000000000000 10110000 left 176
    */

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(leftShiftShouldSucceed);
    RUN_TEST(rightShiftShouldSucceed);
    
    return UNITY_END();
}