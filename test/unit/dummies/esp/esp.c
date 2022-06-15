#include "esp_test.h"
#include <stdbool.h>

bool commandSucceed = true;

ESP_Status_t ESP_Status = {.ChipStatus = ESP_CHIP_OK,
                           .WIFIStatus = NOT_CONNECTED,
                           .MQTTStatus = NOT_CONNECTED};

void ESP_ReturnTrue(void) { commandSucceed = true; }

void ESP_ReturnFalse(void) { commandSucceed = false; }

void ESP_Init(void) {}

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return commandSucceed;
}

bool ESP_SendCommandForce(char *cmd, char *expectedResponse, int timeoutMs) {
    return commandSucceed;
}

void ESP_SoftReset(void) {}

bool ESP_CheckIsResponding(void) { return commandSucceed; }

void ESP_SetMQTTReceiverFunction(void (*receive)(char *)) {}
