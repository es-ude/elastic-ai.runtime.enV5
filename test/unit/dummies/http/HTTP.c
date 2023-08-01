#include "HTTP.h"
#include <stddef.h>
#include <string.h>
#include "CException.h"

HttpResponse_t *HTTPResponse = NULL;

void HTTPReceive(char *httpResponse) {}

void HTTPGet(const char *url, HttpResponse_t **data) {

    char response[] = "Success";
    HTTPResponse->response = (uint8_t *)response;
    HTTPResponse->length = strlen(response);
    data = &HTTPResponse;
}

void HTTPCleanResponseBuffer(HttpResponse_t **response) {}

void HTTPSetReceiverFunction(void) {}
