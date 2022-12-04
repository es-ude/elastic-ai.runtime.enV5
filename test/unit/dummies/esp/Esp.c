#include "Esp.h"
#include "EspUnitTest.h"
#include <stdbool.h>

volatile espStatus_t espStatus = {
    .ChipStatus = ESP_CHIP_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};


void espInit(void) {}

espErrorCode_t espSendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return ESPDUMMY_RETURN_CODE;
}

void espInternalSoftReset(void) {}

bool espInternalCheckIsResponding(void) {
    return ESPDUMMY_RETURN_CODE;
}

void espSetMqttReceiverFunction(void (*receive)(char *)) {}

void espSetHTTPReceiverFunction(void (*receive)(char *)) {}
