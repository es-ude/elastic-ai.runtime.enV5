// TODO: add sufficient testing

#define SOURCE_FILE "FPGA-CONFIG-UTEST"

#include "FlashUnitTest.h"
#include "httpDummy.h"
#include "unity.h"

char url[] = "http://test.me.domain/";

void setUp(void) {
    flashSetUpDummyStorage(FLASH_BYTES_PER_SECTOR);
}

void tearDown(void) {
    flashRemoveDummyStorage();
}

void test_downloadViaHttpUrlToLong() {
    TEST_FAIL();
}
void test_downloadViaHttpUrlCorrect() {
    TEST_FAIL();
}
void test_downloadViaHttpFailedToConnect() {
    TEST_FAIL();
}
void test_downloadViaHttpRetryOnError() {
    TEST_FAIL();
}
void test_downloadViaHttpOrderCorrect() {
    TEST_FAIL();
}
void test_downloadViaHttpSuccessful() {
    TEST_FAIL();
}

void test_donwloadViaUsbProtocolCorrect() {
    TEST_FAIL();
}
void test_downloadViaUsbOrderCorrect() {
    TEST_FAIL();
}
void test_downloadViaUsbSuccessful() {
    TEST_FAIL();
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_downloadViaHttpUrlToLong);
    RUN_TEST(test_downloadViaHttpUrlCorrect);
    RUN_TEST(test_downloadViaHttpFailedToConnect);
    RUN_TEST(test_downloadViaHttpRetryOnError);
    RUN_TEST(test_downloadViaHttpSuccessful);
    RUN_TEST(test_downloadViaHttpOrderCorrect);

    RUN_TEST(test_donwloadViaUsbProtocolCorrect);
    RUN_TEST(test_downloadViaUsbOrderCorrect);
    RUN_TEST(test_downloadViaUsbSuccessful);

    return UNITY_END();
}