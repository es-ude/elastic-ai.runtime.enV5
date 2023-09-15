#ifndef ENV5_HTTP_H
#define ENV5_HTTP_H

#include <stdint.h>

typedef enum HttpExceptions {
    HTTP_RESPONSE_IS_NULL = 0x01,
    HTTP_CONNECTION_FAILED = 0x02
} HttpExceptions_t;

struct httpResponse {
    uint32_t length;
    uint8_t *response;
};
typedef struct httpResponse HttpResponse_t;

/*! \brief Curls http from url into data
 *
 * \param url url to curl
 * \param data Pointer data is stored to
 */
void HTTPGet(const char *url, HttpResponse_t **data);

/*! \brief only for the Network library
 *
 * Used to sets its function to receive HTTP messages from UART.
 */
void HTTPSetReceiverFunction(void);

/*! \brief cleans buffer
 *
 * @param response buffer to be removed
 */
void HTTPCleanResponseBuffer(HttpResponse_t **response);

#endif // ENV5_HTTP_H
