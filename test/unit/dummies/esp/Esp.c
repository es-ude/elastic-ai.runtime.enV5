#include "eai/network/Esp.h"
#include "EspUnitTest.h"

#include <stdbool.h>
#include <stdio.h>

espErrorCode_t espSendCommandStandard(char *cmd, char *expectedResponse, int timeoutMs);

espErrorCode_t ESPDUMMY_RETURN_CODE = ESP_NO_ERROR;
espSendCommandFunction espSendCommandFunctionToUse = espSendCommandStandard;

void espSetSendCommandFunction(espSendCommandFunction functionToUse) {
    if (functionToUse != NULL) {
        espSendCommandFunctionToUse = functionToUse;
    }
    espSendCommandFunctionToUse = espSendCommandStandard;
}

volatile espStatus_t espStatus = {
    .ChipStatus = ESP_CHIP_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

void espInit(void) {}

espErrorCode_t espSendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return espSendCommandFunctionToUse(cmd, expectedResponse, timeoutMs);
}

espErrorCode_t espSendCommandStandard(char *cmd, char *expectedResponse, int timeoutMs) {
    return ESPDUMMY_RETURN_CODE;
}

void espInternalSoftReset(void) {}

bool espInternalCheckIsResponding(void) {
    return ESPDUMMY_RETURN_CODE;
}

void espSetMqttReceiverFunction(void (*receive)(char *)) {}

void espSetHTTPReceiverFunction(void (*receive)(char *)) {}
