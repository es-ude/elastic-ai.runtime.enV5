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
    00000000 00000000 00000000 00000010 rightShiftThree 2
    00000000 00000000 00000000 10110000 leftShiftThree 176
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
    00000100 00000000 0000000 00000000 rightShiftFive 67 108 864
    00000000 00000000 0000010 11000000 leftShiftFive 704
    */

void leftRotateInp22Num5(void) {
    uint32_t actual = leftRotate(22, 5);
    uint32_t expected = 704;
    TEST_ASSERT_EQUAL(expected, actual);
}
void rightRotateInp22Num5(void) {
    uint32_t actual = rightRotate(22, 5);
    uint32_t expected = 2952790016;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*
    00000000 00000000 00000000 00010110 start 22
    10110000 00000000 00000000 00000000 rightRotateFive 2952790016
    00000000 00000000 00000010 11000000 leftRotateFive 704
*/

void leftRotateInp2147483670Num3(void){
    uint32_t actual = leftRotate(2147483670, 3);
    uint32_t expected = 180;
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

void xorTrueTrue(void){
    int actual = xor(1,1);
    int expected = 0;
    TEST_ASSERT_EQUAL(expected, actual);
}
void xorFalseFalse(void){
    int actual = xor(0,0);
    int expected = 0;
    TEST_ASSERT_EQUAL(expected, actual);
}
void xorTrueFalse(void){
    int actual = xor(1,0);
    int expected = 1;
    TEST_ASSERT_EQUAL(expected, actual);
}
void xorFalseTrue(void){
    int actual = xor(0,1);
    int expected = 1;
    TEST_ASSERT_EQUAL(expected, actual);
}


int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(leftShiftInp22Num3);
    RUN_TEST(rightShiftInp22Num3);
    RUN_TEST(leftShiftInp2147483670Num5);
    RUN_TEST(rightShiftInp2147483670Num5);
    RUN_TEST(leftRotateInp2147483670Num3);
    RUN_TEST(rightRotateInp2147483670Num3);
    RUN_TEST(leftRotateInp22Num5);
    RUN_TEST(rightRotateInp22Num5);
    RUN_TEST(xorTrueTrue);
    RUN_TEST(xorFalseFalse);
    RUN_TEST(xorTrueFalse);
    RUN_TEST(xorFalseTrue);

    
    return UNITY_END();
}