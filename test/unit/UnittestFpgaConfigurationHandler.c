#define SOURCE_FILE "FPGA-CONFIG-UTEST"

#include "malloc.h"

#include "unity.h"

#include "FlashUnitTest.h"
#include "FpgaConfigurationHandler.h"
#include "httpDummy.h"

char baseUrl[] = "http://test.me.domain";
uint8_t urlRequestCounter;

void HttpGetCheckUrl(const char *url, HttpResponse_t **data) {
    char *expectedUrl = malloc(sizeof(baseUrl) + 3);
    sprintf(expectedUrl, "%s/%i", baseUrl, urlRequestCounter);

    HttpResponse_t *emptyResponse = malloc(sizeof(HttpResponse_t));
    emptyResponse->length = 0;
    emptyResponse->response = NULL;
    *data = emptyResponse;

    TEST_ASSERT_EQUAL_STRING(expectedUrl, url);
    urlRequestCounter++;
}
void HttpGetReturnDummyChunk(const char *url, HttpResponse_t **data) {
    uint8_t *dummyData = calloc(1, FLASH_BYTES_PER_PAGE);
    dummyData[0] = urlRequestCounter;

    HttpResponse_t *httpResponse = malloc(sizeof(HttpResponse_t));
    httpResponse->length = FLASH_BYTES_PER_PAGE;
    httpResponse->response = dummyData;

    *data = httpResponse;
    urlRequestCounter++;
}

static void checkFlashData(size_t iterations) {
    for (uint8_t index = 0; index < iterations; index++) {
        uint8_t readData;
        data_t readBuffer = {.length = 1, .data = &readData};

        flashReadData(0x00 + FLASH_BYTES_PER_PAGE * index, &readBuffer);

        TEST_ASSERT_EQUAL_UINT8(index, readData);
    }
}

void setUp(void) {
    flashSetUpDummyStorage(FLASH_BYTES_PER_SECTOR);
    urlRequestCounter = 0;
}

void tearDown(void) {
    flashRemoveDummyStorage();
}

void test_downloadViaHttpUrlCorrect() {
    size_t pages = 15;
    httpGetFunctionToUse = HttpGetCheckUrl;
    fpgaConfigurationHandlerDownloadConfigurationViaHttp(baseUrl, pages * FLASH_BYTES_PER_PAGE,
                                                         0x00);
}
void test_downloadViaHttpOrderCorrect() {
    size_t pages = 15;
    httpGetFunctionToUse = HttpGetReturnDummyChunk;
    fpgaConfigurationHandlerDownloadConfigurationViaHttp(baseUrl, pages * FLASH_BYTES_PER_PAGE,
                                                         0x00);
    checkFlashData(pages);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_downloadViaHttpUrlCorrect);
    RUN_TEST(test_downloadViaHttpOrderCorrect);

    return UNITY_END();
}