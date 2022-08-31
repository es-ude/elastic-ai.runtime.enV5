#define SOURCE_FILE "ESP"

#include "esp.h"
#include "TaskWrapper.h"
#include "at_commands.h"
#include "common.h"
#include "esp_internal.h"
#include "uartToESP.h"
#include "uart_configuration.h"
#include <stdbool.h>

/* region VARIABLES */

volatile ESP_Status_t ESP_Status = {
    .ChipStatus = ESP_CHIP_NOT_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

void esp_Init(void) {
    // initialize uart interface for AT commands
    uartToEsp_Init(&uartToEspDevice);

    // check if ESP module is available
    while (!checkIsResponding()) {
        PRINT("ESP NOT responding. Trying again ...")
        TaskSleep(1000);
    }
    PRINT_DEBUG("ESP responding.")

    // reset ESP configuration
    softReset();

    // configure ESP
    while (ESP_NO_ERROR != esp_SendCommand(AT_DISABLE_ECHO, AT_DISABLE_ECHO_RESPONSE, 100)) {
        PRINT("Could not disable ESP echoing commands! Trying again ...")
    }
    PRINT_DEBUG("Disabled ESP echoing commands successful.")
    while (ESP_NO_ERROR !=
           esp_SendCommand(AT_DISABLE_MULTI_CONNECT, AT_DISABLE_MULTI_CONNECT_RESPONSE, 100)) {
        PRINT("Could not set ESP to single connection mode! Trying again ...")
    }
    PRINT_DEBUG("Set ESP to single connection mode successful.")

    // set ESP status
    ESP_Status.ChipStatus = ESP_CHIP_OK;
    ESP_Status.WIFIStatus = NOT_CONNECTED;
    ESP_Status.MQTTStatus = NOT_CONNECTED;
}

esp_errorCode esp_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    uartToEsp_errorCode uartToEspErrorCode = uartToESP_sendCommand(cmd, expectedResponse);

    if (uartToEspErrorCode == UART_IS_BUSY) {
        PRINT("Only one ESP command at a time can be send, did not send %s.", cmd)
        return ESP_UART_IS_BUSY;
    }

    // check for response
    bool responseArrived = false;
    TaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        responseArrived = uartToESP_correctResponseArrived();
        if (responseArrived) {
            PRINT_DEBUG("Correct response received!")
            break;
        }
        TaskSleep(REFRESH_RESPOND_IN_MS);
    }

    // free command buffer of UART
    uartToESP_freeCommandBuffer();

    return responseArrived ? ESP_NO_ERROR : ESP_WRONG_ANSWER_RECEIVED;
}

void esp_SetMQTTReceiverFunction(void (*receive)(char *)) {
    uartToESP_SetMQTTReceiverFunction(receive);
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

bool checkIsResponding(void) {
    esp_errorCode espErrorCode = esp_SendCommand("AT", "OK", 100);
    return espErrorCode == ESP_NO_ERROR ? true : false;
}

bool softReset(void) {
    esp_errorCode espErrorCode = esp_SendCommand(AT_RESTART, AT_RESTART_RESPONSE, 1000);
    if (espErrorCode == ESP_NO_ERROR) {
        PRINT_DEBUG("ESP reset successful. Sleeping for 2 seconds.")
        TaskSleep(2000); // wait until the esp is ready
    } else {
        PRINT("ESP reset failed!")
    }

    return espErrorCode;
}

/* endregion */