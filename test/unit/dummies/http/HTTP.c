#include "HTTP.h"
#include "CException.h"
#include "httpDummy.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

HttpResponse_t *HTTPResponse = NULL;

httpGetFunction httpGetFunctionToUse = HttpGetSuccessful;
void HTTPGet(const char *url, HttpResponse_t **data) {
    httpGetFunctionToUse(url, data);
}

void HttpGetSuccessful(const char *url, HttpResponse_t **data) {
    char response[] = "Success";
    HTTPResponse->response = (uint8_t *)response;
    HTTPResponse->length = strlen(response);
    data = &HTTPResponse;
}
void HttpGetConnectionFails(const char *url, HttpResponse_t **data) {
    Throw(HTTP_CONNECTION_FAILED);
}
void HttpGetResponseNull(const char *url, HttpResponse_t **data) {
    Throw(HTTP_WRONG_RESPONSE);
}

void HTTPCleanResponseBuffer(HttpResponse_t *response) {
    if (response == NULL) {
        return;
    }
    if (response->response != NULL) {
        free(response->response);
    }
    free(response);
}
void HTTPReceive(char *httpResponse) {
    // Just here to satisfy the compiler
}
void HTTPSetReceiverFunction(void) {
    // Just here to satisfy the compiler
}
