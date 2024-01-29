#define SOURCE_FILE "HTTP"

#include <stdlib.h>
#include <string.h>

#include "CException.h"

#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include "HTTP.h"

HttpResponse_t *HTTPResponse = NULL;
volatile uint32_t httpCount = 0;

void HTTPGet(const char *url, HttpResponse_t **data) {
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK || espStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT_DEBUG("HTTP ERROR - No connection")
        Throw(HTTP_CONNECTION_FAILED);
    }

    if (strlen(url) > 256) {
        PRINT_DEBUG("HTTP ERROR - URL to long");
        Throw(HTTP_URL_TO_LONG);
    }

    size_t lengthOfString = AT_HTTP_GET_LENGTH + strlen(url);
    char httpGet[lengthOfString];
    snprintf(httpGet, lengthOfString, AT_HTTP_GET, url);

    if (espSendCommand(httpGet, AT_HTTP_GET_RESPONSE, 10000) == ESP_WRONG_ANSWER_RECEIVED) {
        HTTPCleanResponseBuffer(HTTPResponse);
        Throw(HTTP_WRONG_RESPONSE);
    }

    *data = HTTPResponse;
    HTTPResponse = NULL;
}

void HTTPCleanResponseBuffer(HttpResponse_t *response) {
    if (response == NULL) {
        return;
    }

    free(response->response);
    free(response);
    response = NULL;
}

void HTTPReceive(char *httpResponse) {
    char *startSize = strstr(httpResponse, ":") + 1;
    char *endSize = strstr(httpResponse, ",");
    uint32_t bytesOfData = strtol(startSize, &endSize, 10);

    char *data = strstr(startSize, ",") + 1;

    HTTPResponse = calloc(1, sizeof(HttpResponse_t));
    HTTPResponse->length = bytesOfData;
    HTTPResponse->response = calloc(bytesOfData, sizeof(uint8_t));
    memcpy(HTTPResponse->response, data, bytesOfData);
}
void HTTPSetReceiverFunction(void) {
    PRINT_DEBUG("Receive function set")
    espSetHTTPReceiverFunction(HTTPReceive);
}
