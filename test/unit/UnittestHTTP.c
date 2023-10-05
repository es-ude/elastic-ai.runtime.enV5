#include "CException.h"
#include "Esp.h"
#include "EspUnitTest.h"
#include "HTTP.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>

void setUp() {
    ESPDUMMY_RETURN_CODE = ESP_WRONG_ANSWER_RECEIVED;
}

void tearDown() {}

static char *generateString(int lengthOfString) {
    char *generatedString = malloc(sizeof('a') * lengthOfString);
    for (int i = 0; i < lengthOfString; i++) {
        generatedString[i] = 'a';
    }
    return generatedString;
}

void test_HttpEmptyBufferReturnsOnNullInput(void) {
    HttpResponse_t *testResponse = NULL;
    HTTPCleanResponseBuffer(testResponse);
    TEST_ABORT();
}

void test_HTTPConnectionFailedThrowsException(void) {
    espStatus.ChipStatus = ESP_CHIP_NOT_OK;
    espStatus.WIFIStatus = NOT_CONNECTED;

    CEXCEPTION_T e;
    Try {
        HTTPGet(NULL, NULL);
        TEST_FAIL_MESSAGE("Should have thrown HTTP_CONNECTION_FAILED!");
    }
    Catch(e) {
        TEST_ASSERT_EQUAL(HTTP_CONNECTION_FAILED, e);
    }
}

void test_HTTPURLtoLongThrowsException(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    char *url = generateString(257);

    CEXCEPTION_T e;
    Try {
        HTTPGet(url, NULL);
        TEST_FAIL_MESSAGE("Should have thrown HTTP_CONNECTION_FAILED!");
    }
    Catch(e) {
        TEST_ASSERT_EQUAL(HTTP_URL_TO_LONG, e);
    }
}
void test_HTTPwrongCommand(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    HttpResponse_t *ptr;
    HttpResponse_t **data = &ptr;
    char *url = generateString(256);

    CEXCEPTION_T e;
    Try {
        HTTPGet(url, data);
        TEST_FAIL_MESSAGE("Should have thrown HTTP_CONNECTION_FAILED!");
    }
    Catch(e) {
        TEST_ASSERT_EQUAL(HTTP_WRONG_RESPONSE, e);
    }
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_HttpEmptyBufferReturnsOnNullInput);
    RUN_TEST(test_HTTPConnectionFailedThrowsException);
    RUN_TEST(test_HTTPURLtoLongThrowsException);
    RUN_TEST(test_HTTPwrongCommand);

    return UNITY_END();
}
