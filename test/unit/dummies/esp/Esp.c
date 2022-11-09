#include "Esp.h"
#include "EspUnitTest.h"
#include <stdbool.h>

espErrorCode_t commandSucceed = ESP_NO_ERROR;

volatile espStatus_t espStatus = {
    .ChipStatus = ESP_CHIP_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

void espUnitTestSetErrorCode(espErrorCode_t espErrorCode) {
    commandSucceed = espErrorCode;
}

void espInit(void) {}

espErrorCode_t espSendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return commandSucceed;
}

void espInternalSoftReset(void) {}

bool espInternalCheckIsResponding(void) {
    return commandSucceed;
}

void espSetMqttReceiverFunction(void (*receive)(char *)) {}

void espSetHTTPReceiverFunction(void (*receive)(char *)) {}
