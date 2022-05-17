#define SOURCE_FILE "ESP"

#include <stdbool.h>

#include "esp.h"

#include "common.h"
#include "TaskWrapper.h"
#include "uartToESP.h"


extern esp_command command;

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    if (command.cmd != NULL) {
        PRINT("Only one ESP command at a time can be send, did not send %s.", cmd)
        return false;
    }

    command.cmd = cmd;
    command.responseArrived = false;
    command.expectedResponse = expectedResponse;

    uartToESP_SendCommand();

    TaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        if (command.responseArrived)
            break;
        TaskSleep(REFRESH_RESPOND_IN_MS);
    }

    command.cmd = NULL;
    return command.responseArrived;
}

void ESP_SoftReset() {
    ESP_SendCommand("AT+RST", "OK", 1000);
    TaskSleep(2000); // wait until the esp is ready
}

bool ESP_CheckIsResponding() {
    return ESP_SendCommand("AT", "OK", 100);
}
