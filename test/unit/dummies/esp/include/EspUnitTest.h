#ifndef ENV5_ESP_UNITTEST_HEADER
#define ENV5_ESP_UNITTEST_HEADER

#include "Esp.h"

extern espErrorCode_t ESPDUMMY_RETURN_CODE;

typedef espErrorCode_t (*espSendCommandFunction)(char *cmd, char *expectedResponse, int timeoutMs);
void espSetSendCommandFunction(espSendCommandFunction functionToUse);

#endif /* ENV5_ESP_UNITTEST_HEADER */
