#define SOURCE_FILE "HTTP"

#include "http.h"
#include "at_commands.h"
#include "common.h"
#include "esp.h"
#include <stdlib.h>
#include <string.h>

char *HTTP_response = NULL;

void HTTP_receive(char *header) {
    header = strstr(header, ",");
    header++;
    HTTP_response = malloc(sizeof(char) * strlen(header));
    strcpy(HTTP_response, header);
}

HTTPStatus HTTP_get(const char *url, char **data) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK || ESP_Status.WIFIStatus == NOT_CONNECTED) {
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

    if (esp_SendCommand(httpGet, AT_HTTP_GET_RESPONSE, 10000) == ESP_WRONG_ANSWER_RECEIVED) {
        if (HTTP_response != NULL) {
            free(HTTP_response);
        }
        return HTTP_CONNECTION_FAILED;
    }

    *data = malloc(sizeof(char) * strlen(HTTP_response));
    strcpy(*data, HTTP_response);
    free(HTTP_response);
    HTTP_response = NULL;
    return HTTP_SUCCESS;
}

void HTTP_init(void) {
    esp_SetHTTPReceiverFunction(HTTP_receive);
}
