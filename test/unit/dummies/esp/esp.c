#include "esp_test.h"
#include <stdbool.h>

bool commandSucceed = true;

ESP_Status_t ESP_Status = {
    .ChipStatus = ESP_CHIP_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

void esp_ReturnTrue(void) {
    commandSucceed = true;
}

void esp_ReturnFalse(void) {
    commandSucceed = false;
}

void esp_Init(void) {}

bool esp_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return commandSucceed;
}

void esp_SoftReset(void) {}

bool esp_CheckIsResponding(void) {
    return commandSucceed;
}

void esp_SetMQTTReceiverFunction(void (*receive)(char *)) {}
