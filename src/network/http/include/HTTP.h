#ifndef ENV5_HTTP_H
#define ENV5_HTTP_H

#include <stdint.h>

typedef enum HttpExceptions {
    HTTP_NO_ERROR = 0x00,
    HTTP_CONNECTION_FAILED,
    HTTP_URL_TO_LONG,
    HTTP_WRONG_RESPONSE,
} HttpExceptions_t;

struct httpResponse {
    uint32_t length;
    uint8_t *response;
};
typedef struct httpResponse HttpResponse_t;

/*!
 * @brief Curls http from url into data
 *
 * @param url url to curl
 * @param data Pointer data is stored to
 *
 * @throws HTTP_CONNECTION_FAILED if we can't communicate with the esp chip
 * @throws HTTP_URL_TO_LONG if the url exceeds the limit of 256 characters
 * @throws HTTP_WRONG_RESPONSE if response from esp chip is invalid
 */
void HTTPGet(const char *url, HttpResponse_t **data);

/*!
 * @brief only for the Network library
 *
 * Used to sets its function to receive HTTP messages from UART.
 */
void HTTPSetReceiverFunction(void);

/*!
 * @brief cleans the response buffer
 *
 * @param response buffer to be removed
 */
void HTTPCleanResponseBuffer(HttpResponse_t *response);

#endif // ENV5_HTTP_H
