#define SOURCE_FILE "esp Base Library"

#include "common.h"
#include "FreeRTOSUtils/TaskWrapper.h"

#include "espBase.h"
#include "espTCP.h"

#include "string.h"

ESP_StatusFlags ESPChipStatusFlags = {
        .ChipStatus = ESP_CHIP_NOT_OK,
        .WIFIStatus = NOT_CONNECTED,
        .TCPStatus = NOT_CONNECTED,
        .MQTTStatus = NOT_CONNECTED};

ESP_StatusFlags ESP_GetStatusFlags() { return ESPChipStatusFlags; }

void ESP_CleanReceiveBuffer() {
    uartToESP_CleanReceiveBuffer();
}

bool MQTT_connected(void) {
    ASSERT(ESPChipStatusFlags.ChipStatus)
    ASSERT(ESPChipStatusFlags.WIFIStatus == CONNECTED)
    ASSERT(ESPChipStatusFlags.MQTTStatus == CONNECTED)
    return true;
}

void ESP_GetIP(void) {
    PRINT("GETTING IP")
    ESP_SendCommand("AT+CIFSR", "#", 5000);
    //PRINT("IP: %s", uartToESP_GetReceiveBuffer())
}

bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs) {
    // Clean the uart receive buffer before send the command
    ESP_CleanReceiveBuffer();
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

static void DisableEcho() { ESP_SendCommand("ATE0", "OK", 100); }

static bool CheckIsESPResponding() { return ESP_SendCommand("AT", "OK", 100); }

bool ESP_Init(bool softInit) {
    // init the uart interface for at
    uartToEsp_Init(esp32UartDevice);
    if (!softInit)
        ESP_SoftReset();
    while (!CheckIsESPResponding()) {
        PRINT("ESP ACK_CHECK failed")
        TaskSleep(1000);
        ESP_SoftReset();
    }
    PRINT("ESP is responding")
    // disable echo, make the output more clean
    DisableEcho();
    // disable multiple connections
    int retrysLeft = 2;
    while (!ESP_SendCommand("AT+CIPMUX=0", "OK", 100) && retrysLeft > 0) {
        PRINT("could not set esp to single connection mode!")
        retrysLeft--;
    }
    ESPChipStatusFlags.ChipStatus = ESP_CHIP_OK;
    ESPChipStatusFlags.WIFIStatus = NOT_CONNECTED;
    ESPChipStatusFlags.TCPStatus = NOT_CONNECTED;
    ESPChipStatusFlags.MQTTStatus = NOT_CONNECTED;
    return true;
}

bool ESP_ConnectToNetwork(ESP_NetworkCredentials credentials) {
    ASSERT(ESPChipStatusFlags.ChipStatus)
    ASSERT(ESPChipStatusFlags.WIFIStatus == NOT_CONNECTED)
    // if the length of the ssid + password cannot be longer than about 90 chars
    char cmd[100];
    strcpy(cmd, "AT+CWJAP=\"");
    strcat(cmd, credentials.ssid);
    strcat(cmd, "\",\"");
    strcat(cmd, credentials.password);
    strcat(cmd, "\"");

    bool response = ESP_SendCommand(cmd, "WIFI GOT IP", 5000);

    if (response) {
        PRINT("Connected to %s", credentials.ssid)
        ESPChipStatusFlags.WIFIStatus = CONNECTED;
    } else {
        PRINT("Failed to connect to %s", credentials.ssid)
        ESPChipStatusFlags.WIFIStatus = NOT_CONNECTED;
    }

    return response;
}

bool ESP_DisconnectFromNetwork() {
    ASSERT(ESPChipStatusFlags.ChipStatus)
    ASSERT(ESPChipStatusFlags.WIFIStatus == CONNECTED)
    ESP_TCP_Close(true);
//    ESP_MQTT_Disconnect(true);
    ESP_SendCommand("AT+CWQAP", "OK", 5000);
    ESPChipStatusFlags.WIFIStatus = NOT_CONNECTED;
    return true;
}

bool ESP_Ping(char *ipAdress) {
    ASSERT(ESPChipStatusFlags.ChipStatus)
    ASSERT(ESPChipStatusFlags.WIFIStatus == CONNECTED)
    char cmd[30];
    strcpy(cmd, "AT+PING=\"");
    strcat(cmd, ipAdress);
    strcat(cmd, "\"");

    bool response = ESP_SendCommand(cmd, "+PING:", 1000);
    if (response) {
        char timeElapsedStr[20];
        uartToESP_Readln("+PING:", timeElapsedStr);
        PRINT("PING host IP %s, %sms", ipAdress, timeElapsedStr)
    } else {
        PRINT("PING Timeout")
    }
    return response;
}
