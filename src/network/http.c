#define SOURCE_FILE "HTTP"

#include "http.h"
#include "Network.h"
#include "common.h"
#include "esp.h"
#include <stdlib.h>
#include <string.h>
#include "at_commands.h"
#include "http.h"

int HTTP_responseLength = 0;
char *HTTP_response = NULL;

typedef struct {
    int length;
    char *target;
    void (*doneCallback)(void);
} esp_read_request;

#define NO_READ_REQUEST ((esp_read_request){.length = 0, .target = NULL, .doneCallback = NULL})

static inline bool consume(char **str, char *expected) {
    int expectedLen = strlen(expected);

    if (!strncmp(*str, expected, expectedLen)) {
        *str += expectedLen;
        return true;
    }

    return false;
}

static inline bool consumeUntil(char **str, char *endMarker, int *consumed) {
    char *end = strstr(*str, endMarker);
    if (end == NULL)
        return false;

    *consumed = end - *str;
    *str = end + strlen(endMarker);
    return true;
}

void HTTP_tryParseHeader(char *header) {
    if (!consume(&header, "+HTTPCLIENT:"))
        return;

    char *length = header;
    int lengthLength;
    if (!consumeUntil(&header, ",", &lengthLength))
        return;

    if (*header != '\0')
        return;

    int chunkLength = strtol(length, NULL, 10);

    int currentLength = HTTP_responseLength;
    HTTP_responseLength += chunkLength;

    HTTP_response = HTTP_response == NULL ? malloc(HTTP_responseLength)
                                          : realloc(HTTP_response, HTTP_responseLength);

    //    return (esp_read_request){.length = chunkLength,
    //                              .target = &HTTP_response[currentLength],
    //                              .doneCallback = HTTP_finishChunk};
}

HTTPStatus HTTP_get(const char *url, char **data, int *dataLength) {
    *data = NULL;
    *dataLength = 0;

    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK || ESP_Status.WIFIStatus == NOT_CONNECTED) {
        return HTTP_CONNECTION_FAILED;
    }

    if (strlen(url) > 256) {
        if (strlen(url) > 8192) {
            return HTTP_CONNECTION_FAILED;
        }

        // TODO AT+HTTPURLCFG
        return HTTP_CONNECTION_FAILED;
    }

    size_t lengthOfString = AT_HTTP_GET_LENGTH + strlen(url);
    char *httpGet = malloc(lengthOfString);
    snprintf(httpGet, lengthOfString, AT_HTTP_GET, url, !strncmp("https", url, 5) ? "2" : "1");

    if (!esp_SendCommand(httpGet, AT_HTTP_GET_RESPONSE, 10000)) {
        if (HTTP_response != NULL)
            free(HTTP_response);
        return HTTP_CONNECTION_FAILED;
    }

    *dataLength = HTTP_responseLength;
    *data = HTTP_response;

    HTTP_responseLength = 0;
    HTTP_response = NULL;

    return HTTP_SUCCESS;
}

void HTTP_init(void) {
    esp_SetHTTPReceiverFunction(HTTP_tryParseHeader);
}
