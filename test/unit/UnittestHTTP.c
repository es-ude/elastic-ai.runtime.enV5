#include "CException.h"
#include "Esp.h"
#include "HTTP.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void test_HTTPCleanResponseBufferShouldThrowException(void) {
    HttpResponse_t *response = NULL;
    CEXCEPTION_T e;
    Try {
        HTTPCleanResponseBuffer(response);
        TEST_FAIL_MESSAGE("Should have thrown!");
    }
    Catch(e) {
        TEST_ASSERT_EQUAL(RESPONSE_IS_NULL, e);
    }
}

void test_HTTPConnectionFailedThrowsException(void) {
    TEST_FAIL();
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_HTTPCleanResponseBufferShouldThrowException);
    RUN_TEST(test_HTTPConnectionFailedThrowsException);
    
    return UNITY_END();
}
