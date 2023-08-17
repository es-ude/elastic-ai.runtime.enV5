#include <stdlib.h>
#include <string.h>
#include "CException.h"
#include "Esp.h"
#include "EspUnitTest.h"
#include "HTTP.h"
#include "unity.h"


void setUp() {
    ESPDUMMY_RETURN_CODE = ESP_WRONG_ANSWER_RECEIVED;
}

void tearDown() {}


char *generateString(int lengthOfString){
    char *generatedString = malloc(sizeof('a') * lengthOfString);
    for(int i=0; i<lengthOfString; i++){
        generatedString[i] = 'a';
    }
    return generatedString;
}

void test_generateString1(void){
    TEST_ASSERT_EQUAL(1,strlen(generateString(1)));
}

void test_generateString10(void){
    TEST_ASSERT_EQUAL(10,strlen(generateString(10)));
}

void test_generateString100(void){
    TEST_ASSERT_EQUAL(100,strlen(generateString(100)));
}

void test_generateString1000(void){
    TEST_ASSERT_EQUAL(1000,strlen(generateString(1000)));
}

void test_HTTPCleanResponseBufferShouldThrowException(void) {
    HttpResponse_t *response = NULL;
    CEXCEPTION_T e;
    Try {
        HTTPCleanResponseBuffer(response);
        TEST_FAIL_MESSAGE("Should have thrown RESPONSE_IS_NULL!");
    }
    Catch(e) {
        TEST_ASSERT_EQUAL(HTTP_RESPONSE_IS_NULL, e);
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
    char *url = generateString(257);
    
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
    char *url = generateString(256);
    
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

    RUN_TEST(test_generateString1);
    RUN_TEST(test_generateString10);
    RUN_TEST(test_generateString100);
    RUN_TEST(test_generateString1000);
    RUN_TEST(test_HTTPCleanResponseBufferShouldThrowException);
    RUN_TEST(test_HTTPConnectionFailedThrowsException);
    RUN_TEST(test_HTTPURLtoLongThrowsException);
    RUN_TEST(test_HTTPwrongCommand);
    
    return UNITY_END();
}
