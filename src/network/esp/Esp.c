#define SOURCE_FILE "ESP"

#include <stdbool.h>

#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include "EspInternal.h"
#include "FreeRtosTaskWrapper.h"
#include "Uart.h"

/* region VARIABLES */

volatile espStatus_t espStatus = {
    .ChipStatus = ESP_CHIP_NOT_OK, .WIFIStatus = NOT_CONNECTED, .MQTTStatus = NOT_CONNECTED};

// FIXME: pass this from the outside, because it changes with the hardware?
static uartDevice_t uartDevice = {
    .name = "uart_to_esp32",
    // region HARDWARE CONFIGURATION
    // IMPORTANT: Should be modified according to the hardware
    .uartId = 1,
    .txPin = 4,
    .rxPin = 5,
    // endregion HARDWARE CONFIGURATION
    // region ESP32 FIRMWARE
    .baudrateSet = 115200,
    .baudrateActual = 0,
    .dataBits = 8,
    .stopBits = 1,
    .parity = NoneParity
    // endregion ESP32 FIRMWARE
};

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

void espInit(void) {
    /* 1. Initializes UART connection
     * 2. resets the ESP
     * 3. check if the connection is working (by waiting for response)
     * 4. echo-command is disabled
     * 5. single connection mode is enabled
     * 6. ChipStatus of `espStatus` is set to `ESP_CHIP_OK`.
     */

    // initialize uart interface for AT commands
    uartInit(&uartDevice);

    // check if ESP module is available
    while (!espInternalCheckIsResponding()) {
        PRINT("ESP NOT responding. Trying again ...");
        freeRtosTaskWrapperTaskSleep(1000);
    }
    PRINT_DEBUG("ESP responding.");

    // reset ESP configuration
    espInternalSoftReset();

    // configure ESP
    while (ESP_NO_ERROR != espSendCommand(AT_DISABLE_ECHO, AT_DISABLE_ECHO_RESPONSE, 100)) {
        PRINT("Could not disable ESP echoing commands! Trying again ...");
    }
    PRINT_DEBUG("Disabled ESP echoing commands successful.");
    while (ESP_NO_ERROR !=
           espSendCommand(AT_DISABLE_MULTI_CONNECT, AT_DISABLE_MULTI_CONNECT_RESPONSE, 100)) {
        PRINT("Could not set ESP to single connection mode! Trying again ...");
    }
    PRINT_DEBUG("Set ESP to single connection mode successful.");

    // set ESP status
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = NOT_CONNECTED;
    espStatus.MQTTStatus = NOT_CONNECTED;
}

espErrorCode_t espSendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    uartErrorCode_t uartToEspErrorCode = uartSendCommand(cmd, expectedResponse);
    if (uartToEspErrorCode == UART_IS_BUSY) {
        PRINT("Only one ESP command at a time can be send, did not send %s.", cmd);
        return ESP_UART_IS_BUSY;
    }

    // check for response
    bool responseArrived = false;
    freeRtosTaskWrapperTaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        responseArrived = uartCorrectResponseArrived();
        if (responseArrived) {
            PRINT_DEBUG("Correct response received!");
            break;
        }
        PRINT_DEBUG("No Response Received");
        freeRtosTaskWrapperTaskSleep(REFRESH_RESPOND_IN_MS);
    }
    // free command buffer of UART
    uartFreeCommandBuffer();

    return responseArrived ? ESP_NO_ERROR : ESP_WRONG_ANSWER_RECEIVED;
}

void espSetMqttReceiverFunction(void (*receive)(char *)) {
    uartSetMqttReceiverFunction(receive);
}

void espSetHTTPReceiverFunction(void (*receive)(char *)) {
    uartSetHTTPReceiverFunction(receive);
}
/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

bool espInternalCheckIsResponding(void) {
    espErrorCode_t espErrorCode = espSendCommand("AT", "OK", 100);
    return espErrorCode == ESP_NO_ERROR ? true : false;
}

bool espInternalSoftReset(void) {
    espErrorCode_t espErrorCode = espSendCommand(AT_RESTART, AT_RESTART_RESPONSE, 1000);
    if (espErrorCode == ESP_NO_ERROR) {
        PRINT_DEBUG("ESP reset successful. Sleeping for 2 seconds.");
        freeRtosTaskWrapperTaskSleep(2000); // wait until the esp is ready
    } else {
        PRINT("ESP reset failed!");
    }

    return espErrorCode;
}

/* endregion */
