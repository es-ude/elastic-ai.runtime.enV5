#include "BinaryArithmeticAbstraction.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void shouldFail(void) {
    TEST_FAIL();
}

void addShouldSucceed(void) {
    uint8_t actual = add(1,2);
    uint8_t expected = 3;
    TEST_ASSERT_EQUAL(expected, actual);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(shouldFail);
    RUN_TEST(addShouldSucceed);
    
    return UNITY_END();
}