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
        TEST_FAIL_MESSAGE("Should have thrown RESPONSE_IS_NULL!");
    }
    Catch(e) {
        TEST_ASSERT_EQUAL(RESPONSE_IS_NULL, e);
    }
}

void test_HTTPConnectionFailedThrowsException(void) {
    espStatus.ChipStatus = ESP_CHIP_NOT_OK;
    espStatus.WIFIStatus = NOT_CONNECTED;
    CEXCEPTION_T e;
    Try {
        HTTPGet(NULL, NULL);
        TEST_FAIL_MESSAGE("Should have thrown HTTP_CONNECTION_FAILED!");
    }
    Catch(e){
        TEST_ASSERT_EQUAL(HTTP_CONNECTION_FAILED, e);
    }
}

void test_HTTPURLtoLongThrowsException(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    char url[] = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod "
               "tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata s";
    CEXCEPTION_T e;
    Try {
        HTTPGet(url, NULL);
        TEST_FAIL_MESSAGE("Should have thrown HTTP_CONNECTION_FAILED!");
    }
    Catch(e){
        TEST_ASSERT_EQUAL(HTTP_CONNECTION_FAILED, e);
    }
}
void test_HTTPwrongCommand(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    HttpResponse_t *ptr;
    HttpResponse_t **data = &ptr;
    char url[] = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod "
                 "tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimat";
    CEXCEPTION_T e;
    Try {
        HTTPGet(url, data);
        TEST_FAIL_MESSAGE("Should have thrown HTTP_CONNECTION_FAILED!");
    }
    Catch(e){
        TEST_ASSERT_EQUAL(HTTP_CONNECTION_FAILED, e);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_HTTPCleanResponseBufferShouldThrowException);
    RUN_TEST(test_HTTPConnectionFailedThrowsException);
    RUN_TEST(test_HTTPURLtoLongThrowsException);
    RUN_TEST(test_HTTPwrongCommand);
    
    return UNITY_END();
}
