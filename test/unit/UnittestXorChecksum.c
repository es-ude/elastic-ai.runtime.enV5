#define SOURCE_FILE "UNITTEST-XOR"

#include "unity.h"

#include "Checksum.h"

void setUp(void) {}
void tearDown(void) {}

void test_XORCalculationForZeroBytes(void) {
    uint8_t input1 = 0x00, input2 = 0x00;
    uint8_t actual = checksum(input1, input2);
    TEST_ASSERT_EQUAL_UINT8(0x00, actual);
}

void test_XORCalculationForOneBytes(void) {
    uint8_t input1 = 0xFF, input2 = 0xFF;
    uint8_t actual = checksum(input1, input2);
    TEST_ASSERT_EQUAL_UINT8(0x00, actual);
}

void testXORCalculationForAllBitsDifferent(void) {
    uint8_t input1 = 0x00, input2 = 0xFF;
    uint8_t actual = checksum(input1, input2);
    TEST_ASSERT_EQUAL_UINT8(0xFF, actual);

    actual = checksum(input2, input1);
    TEST_ASSERT_EQUAL_UINT8(0xFF, actual);

    input1 = 0xAA;
    input2 = 0x55;
    actual = checksum(input1, input2);
    TEST_ASSERT_EQUAL_UINT8(0xFF, actual);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_XORCalculationForZeroBytes);
    RUN_TEST(test_XORCalculationForOneBytes);
    RUN_TEST(testXORCalculationForAllBitsDifferent);

    return UNITY_END();
}
