#define SOURCE_FILE "ESP"

#include "esp.h"
#include "common.h"
#include "TaskWrapper.h"
#include "uartToESP.h"
#include <stdbool.h>

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

    uartToESP_Free();

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
