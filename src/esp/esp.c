#define SOURCE_FILE "ESP"

#include <stdbool.h>

#include "esp.h"

#include "common.h"
#include "TaskWrapper.h"
#include "uartToESP.h"

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    // Clean the uart receive buffer before send the command
    uartToESP_CleanReceiveBuffer();
    // send the command
    uartToESP_Println(cmd);

    bool responseArrived = false;
    TaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        responseArrived = uartToESP_ResponseArrived(expectedResponse);
        if (responseArrived)
            break;
        TaskSleep(REFRESH_RESPOND_IN_MS);
    }
#ifndef NDEBUG
    if (!responseArrived) {
        PRINT("Command: %s\nResponse: %s", cmd, uartToESP_GetReceiveBuffer())
    }
#endif
    return responseArrived;
}

void ESP_SoftReset() {
    ESP_SendCommand("AT+RST", "OK", 1000);
    TaskSleep(2000); // wait until the esp is ready
}

bool ESP_CheckIsResponding() {
    return ESP_SendCommand("AT", "OK", 100);
}
