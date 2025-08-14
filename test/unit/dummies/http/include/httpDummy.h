#ifndef ENV5_HTTP_DUMMY_H
#define ENV5_HTTP_DUMMY_H

#include "eai/network/HTTP.h"

typedef void (*httpGetFunction)(const char *url, HttpResponse_t **data);
extern httpGetFunction httpGetFunctionToUse;

void HttpGetSuccessful(const char *url, HttpResponse_t **data);
void HttpGetConnectionFails(const char *url, HttpResponse_t **data);
void HttpGetResponseNull(const char *url, HttpResponse_t **data);

#endif // ENV5_HTTP_DUMMY_H
