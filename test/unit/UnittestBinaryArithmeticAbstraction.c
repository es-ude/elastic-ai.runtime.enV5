#include "BinaryArithmeticAbstraction.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void shouldFail(void) {
    TEST_FAIL();
}

void leftShiftInp22Num3(void) {
    uint32_t actual = leftShift(22, 3);
    uint32_t expected = 176;
    TEST_ASSERT_EQUAL(expected, actual);
}
void rightShiftInp22Num3(void) {
    uint32_t actual = rightShift(22, 3);
    uint32_t expected = 2;
    TEST_ASSERT_EQUAL(expected, actual);
}
    /*
    00000000 00000000 00000000 00010110 start 22
    00000000 00000000 00000000 00000010 rightShift 2
    00000000 00000000 00000000 10110000 leftShift 176
    */

void leftShiftInp2147483670Num5(void) {
    uint32_t actual = leftShift(2147483670, 5);
    uint32_t expected = 704;
    TEST_ASSERT_EQUAL(expected, actual);
}
void rightShiftInp2147483670Num5(void) {
    uint32_t actual = rightShift(2147483670, 5);
    uint32_t expected = 67108864;
    TEST_ASSERT_EQUAL(expected, actual);
}
    /*
    10000000 00000000 0000000 00010110 start 2 147 483 670
    00000100 00000000 0000000 00000000 rightShift 67 108 864
    00000000 00000000 0000010 11000000 leftShift 704
    */

void leftRotateInp2147483670Num3(void){
    uint32_t actual = leftRotate(2147483670, 3);
    uint32_t expected = 177; // was 180
    TEST_ASSERT_EQUAL(expected, actual);
}
void rightRotateInp2147483670Num3(void){
    uint32_t actual = rightRotate(2147483670, 3);
    uint32_t expected = 3489660930;
    TEST_ASSERT_EQUAL(expected, actual);
}
    /*
    10000000 00000000 0000000 00010110 start 2 147 483 670
    11010000 00000000 0000000 00000010 rightRotateThree 3 489 660 930
    00000000 00000000 0000000 10110100 leftRotateThree 180
    */


int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(leftShiftInp22Num3);
    RUN_TEST(rightShiftInp22Num3);
    RUN_TEST(leftShiftInp2147483670Num5);
    RUN_TEST(rightShiftInp2147483670Num5);
    RUN_TEST(rightRotateInp2147483670Num3);
    RUN_TEST(leftRotateInp2147483670Num3);
    
    return UNITY_END();
}