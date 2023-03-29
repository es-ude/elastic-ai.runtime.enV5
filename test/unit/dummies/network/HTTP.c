

#include <stddef.h>
#include <string.h>
#include "HTTP.h"
HttpResponse_t *HTTPResponse = NULL;

void HTTPReceive(char *httpResponse) {
}

HTTPStatus HTTPGet(const char *url, HttpResponse_t **data) {
    
    char response[]="Success";
    HTTPResponse->response=(uint8_t*) response;
    HTTPResponse->length=strlen(response);
    data=&HTTPResponse;
    return HTTP_SUCCESS;
}

void HTTPCleanResponseBuffer(HttpResponse_t *response) {

}

void HTTPSetReceiverFunction(void) {
}
