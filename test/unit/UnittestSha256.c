#include "Sha256.h"
#include "unity.h"

/* TEST NOTES:
    * Kommentare direkt über die Testfunktion schreiben
*/

void setUp(void) {}

void tearDown(void) {}

void smallSigmaZeroInp1870659584(void){
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

void smallSigmaZeroInp3338694733(void){
    uint32_t actual = smallSigmaZero(3338694733);
    uint32_t expected = 2675801897;
    TEST_ASSERT_EQUAL(expected, actual);
}
/*  11000111 00000000 01110000 01001101
    --------------------------------
    10011011 10001110 00000000 11100000         rightRotate7
    00011100 00010011 01110001 11000000 XOR     rightRotate18
    00011000 11100000 00001110 00001001 XOR     rightShift3
    --------------------------------
    10011111 01111101 01111111 00101001
 */

void smallSigmaOneInp1870659584(void){
    uint32_t actual = smallSigmaOne(1870659584);
    uint32_t expected = 1825328;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*  01101111 10000000 00000000 00000000         Input
    -----------------------------------
    00000000 00000000 00110111 11000000         rightRotate17
    00000000 00000000 00001101 11110000 XOR     rightRotate19
    00000000 00011011 11100000 00000000 XOR     rightShift10
    ----------------------------------
    00000000 00011011 11011010 00110000
*/

void smallSigmaOneInp3338694733(void) {
    uint32_t actual = smallSigmaOne(3338694733);
    uint32_t expected = 907975548;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*  11000111 00000000 01110000 01001101
    -----------------------------------
    00111000 00100110 11100011 10000000         rightRotate17
    00001110 00001001 10111000 11100000 XOR     rightRotate19
    00000000 00110001 11000000 00011100 XOR     rightShift10
    ___________________________________
    00110110 00011110 10011011 01111100
*/

void bigSigmaZeroInp1870659584(void) {
    uint32_t actual = bigSigmaZero(1870659584);
    uint32_t expected = 467893694;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*         01101111 10000000 00000000 00000000         Input
    *      ----------------------------------
    *      00011011 11100000 00000000 00000000         rightRotate2
    *      00000000 00000011 01111100 00000000 XOR     rightRotate13
    *      00000000 00000000 00000001 10111110 XOR     rightRotate22
    *      -----------------------------------
    *      00011011 11100011 01111101 10111110         expected: 467893694
    */

void bigSigmaZeroInp3338694733(void) {
    uint32_t actual = bigSigmaZero(3338694733);
    uint32_t expected = 4067365644;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*      11000111 00000000 01110000 01001101         Input
*       -----------------------------------
*       01110001 11000000 00011100 00010011          rightRotate2
*       10000010 01101110 00111000 00000011  XOR     rightRotate13
*       00000001 11000001 00110111 00011100  XOR     rightRotate22
*       -----------------------------------
*       11110010 01101111 00010011 00001100         expected 4067365644
 */

void bigSigmaOneInp3338694733(void) {
    uint32_t actual = bigSigmaOne(3338694733);
    uint32_t expected = 3197945644;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*      11000111 00000000 01110000 01001101         Input
 *      -----------------------------------
 *      00110111 00011100 00000001 11000001         rightRotate6
 *      00001001 10111000 11100000 00001110 XOR     rightRotate11
 *      10000000 00111000 00100110 11100011 XOR     rightRotate25
 *      -----------------------------------
 *      10111110 10011100 11000111 00101100         expected 3197945644
 */

void bigSigmaOneInp1870659584(void) {
    uint32_t actual = bigSigmaOne(1870659584);
    uint32_t expected = 3249795127;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*      01101111 10000000 00000000 00000000         Input
 *      -----------------------------------
 *      00000001 10111110 00000000 00000000         rightRotate6
 *      00000000 00001101 11110000 00000000  XOR    rightRotate11
 *      11000000 00000000 00000000 00110111  XOR    rightRotate25
 *      -----------------------------------
 *      11000001 10110011 11110000 00110111         expected 3249795127
 */

void majorityInp1870659584and3249795127and4(void) {
    uint32_t actual = majority(1870659584,3249795127,4);
    uint32_t expected = 1098907652;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*
 *     01101111 10000000 00000000 00000000 a
 *     11000001 10110011 11110000 00110111 b
 *     00000000 00000000 00000000 00000100 c
 *     -----------------------------------
 *     01000001 10000000 00000000 00000000          (a&b)
 *     00000000 00000000 00000000 00000000 XOR      (a&c)
 *     00000000 00000000 00000000 00000100 XOR      (b&c)
 *     -----------------------------------
 *     01000001 10000000 00000000 00000100          expected: 1098907652
 */

void majorityInp4294967289and913408and4067365644(void) {
    uint32_t actual = majority(4294967289, 913408, 4067365644);
    uint32_t expected = 4067422984;
    TEST_ASSERT_EQUAL(expected, actual);
}
    
/*
 * 11111111 11111111 11111111 11111001          a
 * 00000000 00001101 11110000 00000000          b
 * 11110010 01101111 00010011 00001100          c
 * -----------------------------------
 * 00000000 00001101 11110000 00000000          (a&b)
 * 11110010 01101111 00010011 00001000 XOR      (a&c)
 * 00000000 00001101 00010000 00000000 XOR      (b&c)
 * -----------------------------------
 * 11110010 01101111 11110011 00001000          expected: 4067422984
*/

void majorityInp221191136and913408and4067365644(void) {
    uint32_t actual = majority(221191136, 913408, 4067365644);
    uint32_t expected = 3085056;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*
* 00001101 00101111 00011011 11100000          a
* 00000000 00001101 11110000 00000000          b
* 11110010 01101111 00010011 00001100          c
* -----------------------------------
* 00000000 00001101 00010000 00000000          (a&b)
* 00000000 00101111 00010011 00000000 XOR      (a&c)
* 00000000 00001101 00010000 00000000 XOR      (b&c)
* -----------------------------------
* 00000000 00101111 00010011 00000000           expected: 3085056
 */

void choiceInp1870659584and3249795127and4(void) {
    uint32_t actual = choice(1870659584,3249795127,4);
    uint32_t expected = 1098907652;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*
 *     01101111 10000000 00000000 00000000 e
 *     11000001 10110011 11110000 00110111 f
 *     00000000 00000000 00000000 00000100 g
 *     10010000 01111111 11111111 11111111 (notE)
 *     01000001 10000000 00000000 00000000 (e&f)
 *     00000000 00000000 00000000 00000100 (notE&g)
 *     01000001 10000000 00000000 00000100 (e&f) XOR (notE&g) 1098907652
*/

void choiceInp4294967289and913408and4067365644(void) {
    uint32_t actual = choice(4294967289,913408,4067365644);
    uint32_t expected = 913412;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*
*  11111111 11111111 11111111 11111001    e
*  00000000 00001101 11110000 00000000    f
*  11110010 01101111 00010011 00001100    g
 * 00000000 00000000 00000000 00000110    notE
 * 00000000 00001101 11110000 00000000    (e&f)
 * 00000000 00000000 00000000 00000100    (notE&g)
 * 00000000 00001101 11110000 00000100    (e&f) XOR (notE&g) expected: 913412
 */

void choiceInp221191136and913408and4067365644(void) {
    uint32_t actual = choice(221191136,913408,4067365644);
    uint32_t expected = 4065136652;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*
* 00001101 00101111 00011011 11100000    e
* 00000000 00001101 11110000 00000000    f
* 11110010 01101111 00010011 00001100    g
* 11110010 11010000 11100100 00011111    notE
* 00000000 00001101 00010000 00000000    (e&f)
* 11110010 01000000 00000000 00001100    (notE&g)
* 11110010 01001101 00010000 00001100    (e&f) XOR (notE&g) expected: 4065136652
*/


void additionOfWordsInp8and9(void){
    uint32_t actual = additionOfWords(8,9);
    uint32_t expected = 17;
    TEST_ASSERT_EQUAL(expected, actual);
}

void additionOfWordsInp221191136and9(void){
    uint32_t actual = additionOfWords(221191136,9);
    uint32_t expected = 221191145;
    TEST_ASSERT_EQUAL(expected, actual);
}

void additionOfWordsInp221191136and913408(void){
    uint32_t actual = additionOfWords(221191136,913408);
    uint32_t expected = 222104544;
    TEST_ASSERT_EQUAL(expected, actual);
}

void calculationOfWordsOne(void){ //too long with Input within name
    uint32_t actual = calculationOfWords(221191136,
                                         3338694733,
                                         4067365644,
                                         1870659584);
    uint32_t expected = 2671216709;
    TEST_ASSERT_EQUAL(expected, actual);
}

/*
 * smallSigmaZero3338694733 = 2675801897
 * smallSigmaOne1870659584 = 1825328
 (221191136 + 2675801897) % h = 2896993033
 (2896993033 + 4067365644) % h = 2669391381
 (2669391381 + 1825328) % h = 2671216709
 */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(smallSigmaZeroInp1870659584);
    RUN_TEST(smallSigmaZeroInp3338694733);
    RUN_TEST(smallSigmaOneInp1870659584);
    RUN_TEST(smallSigmaOneInp3338694733);
    RUN_TEST(bigSigmaZeroInp1870659584);
    RUN_TEST(bigSigmaZeroInp3338694733);
    RUN_TEST(bigSigmaOneInp1870659584);
    RUN_TEST(bigSigmaOneInp3338694733);
    RUN_TEST(majorityInp1870659584and3249795127and4);
    RUN_TEST(majorityInp4294967289and913408and4067365644);
    RUN_TEST(majorityInp221191136and913408and4067365644);
    RUN_TEST(choiceInp1870659584and3249795127and4);
    RUN_TEST(choiceInp4294967289and913408and4067365644);
    RUN_TEST(choiceInp221191136and913408and4067365644);
    RUN_TEST(additionOfWordsInp8and9);
    RUN_TEST(additionOfWordsInp221191136and9);
    RUN_TEST(additionOfWordsInp221191136and913408);
    RUN_TEST(calculationOfWordsOne);

    return UNITY_END();
}