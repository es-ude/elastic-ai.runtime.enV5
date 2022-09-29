#ifndef ENV5_HTTP_H
#define ENV5_HTTP_H

typedef enum HTTPStatus {
    HTTP_SUCCESS,
    HTTP_CONNECTION_FAILED
} HTTPStatus;

HTTPStatus HTTP_get(const char *url, char **data);

void HTTP_init(void);

#endif // ENV5_HTTP_H
