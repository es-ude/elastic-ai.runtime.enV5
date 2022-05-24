#ifndef TEST_SH_TCP_H
#define TEST_SH_TCP_H

#include <stdbool.h>

bool TCP_Open(char *target, int port);

bool TCP_Close(bool force);

bool TCP_SendData(char *data, int timeoutMs);

char *TCP_GetResponse(void);

void TCP_Closed(void);

#endif //TEST_SH_TCP_H
