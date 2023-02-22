#define SOURCE_FILE "HTTP"

#include "HTTP.h"
#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include <stdlib.h>
#include <string.h>

char *HTTPResponse = NULL;

void HTTPReceive(char *header) {
    header = strstr(header, ":");
    header++;
//    uint8_t i=0;
//    char* size;
    char* body;
//    while(header[i]!=','){
//        size[i]=header[i];
//    }
    uint32_t size=strtol(header, &body, 10);
   // header++;
    body++;
    PRINT_DEBUG("%d", size)
    PRINT_DEBUG("%s", body)
    HTTPResponse = malloc(sizeof(char) * strlen(header));
    strcpy(HTTPResponse, header);
}

HTTPStatus HTTPGet(const char *url, char **data) {
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

void HTTPSetReceiverFunction(void) {
    PRINT_DEBUG("Receiver Func was executed\n");
    espSetHTTPReceiverFunction(HTTPReceive);
}
