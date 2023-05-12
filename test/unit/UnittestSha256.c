#include "Sha256.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void shouldFail(void) {
    TEST_FAIL();
}
void shouldSucceed(void) {
    TEST_IGNORE();
    //TEST_SUCCEED(); wirft warning
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(shouldSucceed);
    
    return UNITY_END();
}