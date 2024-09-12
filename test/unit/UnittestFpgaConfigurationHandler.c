#define SOURCE_FILE "FPGA-CONFIG-UTEST"

#include <stdlib.h>

#include "unity.h"

#include "EnV5HwConfiguration.h"
#include "FlashUnitTest.h"
#include "FpgaConfigurationHandler.h"
#include "httpDummy.h"

flashConfiguration_t flashConfiguration;

char baseUrl[] = "http://test.me.domain";
uint8_t urlRequestCounter;

void HttpGetCheckUrl(const char *url, HttpResponse_t **data) {
    char *expectedUrl = malloc(sizeof(baseUrl) + 33 * sizeof(char));
    sprintf(expectedUrl, "%s?chunkNumber=%u&chunkMaxSize=%u", baseUrl, urlRequestCounter,
            flashGetBytesPerPage());

    HttpResponse_t *emptyResponse = malloc(sizeof(HttpResponse_t));
    emptyResponse->length = 0;
    emptyResponse->response = NULL;
    *data = emptyResponse;

    TEST_ASSERT_EQUAL_STRING(expectedUrl, url);
    urlRequestCounter++;
}
void HttpGetReturnDummyChunk(const char *url, HttpResponse_t **data) {
    uint8_t *dummyData = calloc(1, flashConfiguration.flashBytesPerPage);
    dummyData[0] = urlRequestCounter;

    HttpResponse_t *httpResponse = malloc(sizeof(HttpResponse_t));
    httpResponse->length = flashConfiguration.flashBytesPerPage;
    httpResponse->response = dummyData;

    *data = httpResponse;
    urlRequestCounter++;
}

static void checkFlashData(size_t iterations) {
    for (size_t index = 0; index < iterations; index++) {
        uint8_t readData;
        data_t readBuffer = {.length = 1, .data = &readData};

        flashReadData(&flashConfiguration, 0x00 + flashConfiguration.flashBytesPerPage * index,
                      &readBuffer);

        TEST_ASSERT_EQUAL_UINT8(index, readData);
    }
}

void setUp(void) {
    flashConfiguration.flashSpiConfiguration = NULL;
    flashConfiguration.flashBytesPerPage = flashGetBytesPerPage();
    flashConfiguration.flashBytesPerSector = flashGetBytesPerSector();

    flashSetUpDummyStorage(flashConfiguration.flashBytesPerSector);
    urlRequestCounter = 0;
}

void tearDown(void) {
    flashRemoveDummyStorage();
}

void test_downloadViaHttpUrlCorrect() {
    size_t pages = 15;
    httpGetFunctionToUse = HttpGetCheckUrl;
    fpgaConfigurationHandlerDownloadConfigurationViaHttp(
        &flashConfiguration, baseUrl, pages * flashConfiguration.flashBytesPerPage, 1);
}
void test_downloadViaHttpOrderCorrect() {
    size_t pages = 15;
    httpGetFunctionToUse = HttpGetReturnDummyChunk;
    fpgaConfigurationHandlerDownloadConfigurationViaHttp(
        &flashConfiguration, baseUrl, pages * flashConfiguration.flashBytesPerPage, 1);
    checkFlashData(pages);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_downloadViaHttpUrlCorrect);
    RUN_TEST(test_downloadViaHttpOrderCorrect);

    return UNITY_END();
}
