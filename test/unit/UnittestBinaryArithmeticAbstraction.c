#include "BinaryArithmeticAbstraction.h"
#include "unity.h"
#include <stdbool.h>


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

void leftRotateInp22Num5(void) {                                           //ratation Test with 32
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

void bitwiseXorInp6and3(void) {
    uint32_t actual = bitwiseXor(6, 3);
    uint32_t expected = 5;
    TEST_ASSERT_EQUAL(expected, actual);
}
void bitwiseXorInp2147483670and3489660930(void) {
    uint32_t actual = bitwiseXor(2147483670, 3489660930);
    uint32_t expected = 1342177300;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*
    10000000 00000000 0000000 00010110 XOR          2147483670
    11010000 00000000 0000000 00000010              3489660930
    __________________________________
    01010000 00000000 0000000 00010100              1342177300
*/
void bitwiseNotInp1870659584(void){
    uint32_t actual = bitwiseNot(1870659584);
    uint32_t expected = 2424307711;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  01101111 10000000 00000000 00000000
    10010000 01111111 11111111 11111111
*/

void bitwiseNotInp1870659588(void){
    uint32_t actual = bitwiseNot(1870659588);
    uint32_t expected = 2424307707;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  01101111 10000000 00000000 00000100
    10010000 01111111 11111111 11111011
*/

void sigmaZeroInp1870659584(void){
    uint32_t actual = smallSigmaZero(1870659584);
    uint32_t expected = 221191136;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  01101111 10000000 00000000 00000000       Input
    -----------------------------------
    00000000 11011111 00000000 00000000       rightRotate7
    00000000 00000000 00011011 11100000 XOR   rightRotate18
    00001101 11110000 00000000 00000000 XOR   rightShift3
    -----------------------------------
    00001101 00101111 00011011 11100000
*/

void sigmaZeroInp3338694732(void){
    uint32_t actual = smallSigmaZero(3338694732);
    uint32_t expected = 2675801897;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  11000111 00000000 01110000 01001101       (2^31)+(2^30)+(2^26)+(2^25)+(2^24)+(2^14)+(2^13)+
 (2^12)+(2^6)+(2^3)+(2^2)+(2^0)
    --------------------------------
    10011011 10001110 00000000 11100000         rightRotate7
    00011100 00010011 01110001 11000000 XOR     rightRotate18
    00011000 11100000 00001110 00001001 XOR     rightShift3
    --------------------------------
    10011111 01111101 01111111 00101001  (2^31)+(2^28)+(2^27)+(2^26)+(2^25)+(2^24)+(2^22)+(2^21)+
    (2^20)+(2^19)+(2^18)+(2^16)+(2^14)+(2^13)+(2^12)+(2^11)+(2^10)+(2^9)+(2^8)+(2^5)+(2^3)+(2^0)
 */
void sigmaOneInp1870659584(void){
    uint32_t actual = smallSigmaOne(1870659584);
    uint32_t expected = 1825328;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  01101111 10000000 00000000 00000000       Input (2^30)+(2^29)+(2^27)+(2^26)+(2^25)+(2^24)+(2^23)
    -----------------------------------
    00000000 00000000 00110111 11000000         rightRotate17
    00000000 00000000 00001101 11110000 XOR     rightRotate19
    00000000 00011011 11100000 00000000 XOR     rightShift10
    ----------------------------------
    00000000 00011011 11011010 00110000  (2^4)+(2^5)+(2^9)+(2^11)+(2^12)+(2^14)+(2^15)+(2^16)+
    (2^17)+(2^19)+(2^20)
*/
void sigmaOneInp3338694732(void) {
    uint32_t actual = smallSigmaOne(3338694732);
    uint32_t expected = 907975548;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  11000111 00000000 01110000 01001101       (2^31)+(2^30)+(2^26)+(2^25)+(2^24)+(2^14)+(2^13)+
 (2^12)+(2^6)+(2^3)+(2^2)+(2^0)
    -----------------------------------
    00111000 00100110 11100011 10000000         rightRotate17
    00001110 00001001 10111000 11100000 XOR     rightRotate19
    00000000 00110001 11000000 00011100 XOR     rightShift10
    ___________________________________
    00110110 00011110 10011011 01111100     (2^2)+(2^3)+(2^4)+(2^5)+(2^6)+(2^8)+(2^9)+(2^11)+
    (2^12)+(2^15)+(2^17)+(2^18)+(2^19)+(2^20)+(2^25)+(2^26)+(2^28)+(2^29)
*/

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
    RUN_TEST(bitwiseXorInp6and3);
    RUN_TEST(bitwiseXorInp2147483670and3489660930);
    RUN_TEST(bitwiseNotInp1870659584);
    RUN_TEST(bitwiseNotInp1870659588);
    RUN_TEST(sigmaZeroInp1870659584);
    RUN_TEST(sigmaOneInp1870659584);
    
    return UNITY_END();
}