#include "esp_test.h"
#include <stdbool.h>

bool ESP_DUMMY_RETURN_CODE;

ESP_Status_t ESP_Status = {
    .ChipStatus = ESP_CHIP_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

void ESP_Init(void) {}

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    return ESP_DUMMY_RETURN_CODE;
}

void ESP_SoftReset(void) {}

bool ESP_CheckIsResponding(void) {
    return ESP_DUMMY_RETURN_CODE;
}

void ESP_SetMQTTReceiverFunction(void (*receive)(char *)) {}
