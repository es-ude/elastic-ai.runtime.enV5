#include "Esp.h"
#include "EspUnitTest.h"
#include <stdbool.h>

esp_errorCode commandSucceed = ESP_NO_ERROR;

volatile ESP_Status_t ESP_Status = {
    .ChipStatus = ESP_CHIP_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

void esp_setErrorCode(esp_errorCode espErrorCode) {
    commandSucceed = espErrorCode;
}

void esp_Init(void) {}

esp_errorCode esp_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return commandSucceed;
}

void softReset(void) {}

bool checkIsResponding(void) {
    return commandSucceed;
}

void esp_SetMQTTReceiverFunction(void (*receive)(char *)) {}
