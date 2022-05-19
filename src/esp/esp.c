#define SOURCE_FILE "ESP"

#include <stdbool.h>
#include <string.h>

#include "esp.h"

#include "common.h"
#include "TaskWrapper.h"
#include "uartToESP.h"

extern esp_command command;

inline bool ESP_SendCommandIntern(char *cmd, char *expectedResponse, int timeoutMs) {
    if (strcmp(command.cmd, "\0") != 0) {
        PRINT("Only one ESP command at a time can be send, did not send %s.", cmd)
        return false;
    }

    strcpy(command.cmd, cmd);
    command.responseArrived = false;
    strcpy(command.expectedResponse, expectedResponse);

    uartToESP_SendCommand();

    TaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        if (command.responseArrived)
            break;
        TaskSleep(REFRESH_RESPOND_IN_MS);
    }

    return command.responseArrived;
}

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    bool responseArrived = ESP_SendCommandIntern(cmd, expectedResponse, timeoutMs);
    strcpy(command.cmd, "\0");
    return responseArrived;
}


bool ESP_SendCommandAndGetResponse(char *cmd, char *expectedResponse, int timeoutMs, char *response) {
    bool responseArrived = ESP_SendCommandIntern(cmd, expectedResponse, timeoutMs);
    strcpy(response, command.data);
    strcpy(command.cmd, "\0");
    return responseArrived;
}

void ESP_SoftReset(void) {
    ESP_SendCommand("AT+RST", "OK", 1000);
    TaskSleep(2000); // wait until the esp is ready
}

bool ESP_CheckIsResponding(void) {
    return ESP_SendCommand("AT", "OK", 100);
}
