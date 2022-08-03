#define SOURCE_FILE "ESP"

#include "esp.h"
#include "AT_commands.h"
#include "TaskWrapper.h"
#include "common.h"
#include "uartToESP.h"
#include <stdbool.h>

volatile ESP_Status_t ESP_Status = {
    .ChipStatus = ESP_CHIP_NOT_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

void ESP_Init(void) {
    // initialize uart interface for AT commands
    uartToEsp_Init();

    // check if ESP module is available
    while (!ESP_CheckIsResponding()) {
        PRINT("ESP Check failed, trying again...")
        TaskSleep(1000);
    }
    PRINT_DEBUG("ESP is responding")

    // reset ESP configuration
    ESP_SoftReset();

    // configure ESP
    while (!ESP_SendCommand(AT_DISABLE_ECHO, AT_DISABLE_ECHO_RESPONSE, 100)) {
        PRINT("Could not disable ESP echoing commands!")
    };
    while (!ESP_SendCommand(AT_DISABLE_MULTI_CONNECT, AT_DISABLE_MULTI_CONNECT_RESPONSE, 100)) {
        PRINT("Could not set ESP to single connection mode!")
    }
    PRINT_DEBUG("ESP configured successful")

    // set ESP status
    ESP_Status.ChipStatus = ESP_CHIP_OK;
    ESP_Status.WIFIStatus = NOT_CONNECTED;
    ESP_Status.MQTTStatus = NOT_CONNECTED;
}

bool ESP_CheckIsResponding(void) {
    return ESP_SendCommand("AT", "OK", 100);
}

void ESP_SoftReset(void) {
    ESP_SendCommand(AT_RESTART, AT_RESTART_RESPONSE, 1000);
    TaskSleep(2000); // wait until the esp is ready
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

void ESP_SetMQTTReceiverFunction(void (*receive)(char *)) {
    uartToESP_SetMQTTReceiverFunction(receive);
}
