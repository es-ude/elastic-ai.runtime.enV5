#define SOURCE_FILE "HTTP"

#include "HTTP.h"
#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include <stdlib.h>
#include <string.h>

HttpResponse_t *HTTPResponse = NULL;

void HTTPReceive(char *httpResponse) {
    char *startSize = strstr(httpResponse, ":") + 1;
    char *endSize = strstr(httpResponse, ",");
    uint32_t size = strtol(startSize, &endSize, 10);

    char *body = strstr(startSize, ",") + 1;

    HTTPResponse = malloc(sizeof(HttpResponse_t));
    HTTPResponse->length = size;
    HTTPResponse->response = malloc(sizeof(uint8_t) * size);
    memcpy(HTTPResponse->response, body, size);
}

HTTPStatus HTTPGet(const char *url, HttpResponse_t **data) {
    PRINT_DEBUG("Http get")
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK || espStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT_DEBUG("HTTP ERROR - No connection")
        return HTTP_CONNECTION_FAILED;
    }

    if (strlen(url) > 256) {
        PRINT_DEBUG("HTTP ERROR - URL to long")
        return HTTP_CONNECTION_FAILED;
    }

    size_t lengthOfString = AT_HTTP_GET_LENGTH + strlen(url);
    char *httpGet = malloc(lengthOfString);
    snprintf(httpGet, lengthOfString, AT_HTTP_GET, url);

    if (espSendCommand(httpGet, AT_HTTP_GET_RESPONSE, 10000) == ESP_WRONG_ANSWER_RECEIVED) {
        if (HTTPResponse != NULL) {
            free(HTTPResponse);
        }
        return HTTP_CONNECTION_FAILED;
    }

    *data = HTTPResponse;
    HTTPResponse = NULL;
    return HTTP_SUCCESS;
}

void HTTPCleanResponseBuffer(HttpResponse_t *response) {
    free(response->response);
    free(response);
    response = NULL;
}

void HTTPSetReceiverFunction(void) {
    PRINT_DEBUG("Receiver Func was executed\n")
    espSetHTTPReceiverFunction(HTTPReceive);
}
