#define SOURCE_FILE "ESP"

#include "esp.h"
#include "common.h"
#include "TaskWrapper.h"
#include "uartToESP.h"
#include <stdbool.h>

ESP_Status_t ESP_Status = {
        .ChipStatus = ESP_CHIP_NOT_OK,
        .WIFIStatus = NOT_CONNECTED,
        .MQTTStatus = NOT_CONNECTED};

void ESP_Init(void) {
    // init the uart interface for at
    uartToEsp_Init();

    ESP_SoftReset();

    while (!ESP_CheckIsResponding()) {
        PRINT("ESP Check failed, trying again...")
        TaskSleep(1000);
        ESP_SoftReset();
    }

    PRINT_DEBUG("ESP is responding")
    // disable echo, make the output more clean
    ESP_SendCommand("ATE0", "OK", 100);
    // disable multiple connections
    while (!ESP_SendCommand("AT+CIPMUX=0", "OK", 100)) {
        PRINT("could not set esp to single connection mode!")
    }
    ESP_Status.ChipStatus = ESP_CHIP_OK;
    ESP_Status.WIFIStatus = NOT_CONNECTED;
    ESP_Status.MQTTStatus = NOT_CONNECTED;
}

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    if (uartToESP_IsBusy()) {
        PRINT("Only one ESP command at a time can be send, did not send %s.", cmd)
        return false;
    }

    uartToESP_SendCommand(cmd, expectedResponse);

    bool responseArrived = false;

    TaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        responseArrived = uartToESP_ResponseArrived();
        if (responseArrived)
            break;
        TaskSleep(REFRESH_RESPOND_IN_MS);
    }

    uartToESP_FreeCommand();

    return responseArrived;
}


void ESP_SoftReset(void) {
    ESP_SendCommand("AT+RST", "OK", 1000);
    TaskSleep(2000); // wait until the esp is ready
}

bool ESP_CheckIsResponding(void) {
    return ESP_SendCommand("AT", "OK", 100);
}

void ESP_SetMQTTReceiverFunction(void (*receive)(char *)) {
    uartToESP_SetMQTTReceiverFunction(receive);
}
