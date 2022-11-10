#ifndef ENV5_HTTP_H
#define ENV5_HTTP_H

typedef enum HTTPStatus { HTTP_SUCCESS, HTTP_CONNECTION_FAILED } HTTPStatus;

/*! \brief Curls http from url into data
 *
 * \param url url to curl
 * \param data Pointer data is stored to
 */
HTTPStatus HTTPGet(const char *url, char **data);

/*! \brief only for the Network library
 *
 * Used to sets its function to receive HTTP messages from UART.
 */
void HTTPSetReceiverFunction(void);

#endif // ENV5_HTTP_H
