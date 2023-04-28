#include "BinaryArithmeticAbstraction.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void shouldFail(void) {
    TEST_FAIL();
}

void addShouldSucceed(void) {
    uint8_t actual = add(3,2);
    uint8_t expected = 3;
    TEST_ASSERT_EQUAL(expected, actual);
}

//hier schreibe ich meine Testfunktion rein, in der ich die zu testende Funktion aufrufe

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(addShouldSucceed);
    
    
    return UNITY_END();
}