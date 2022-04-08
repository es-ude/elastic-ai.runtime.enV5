#define SOURCE_FILE "esp TCP Library"

#include "common.h"
#include "FreeRTOSUtils/TaskWrapper.h"

#include "espBase.h"
#include "espTCP.h"

#include "string.h"
#include "stdlib.h"


bool ESP_TCP_Open(char *target, char *port) {
    ASSERT(ESPChipStatusFlags.ChipStatus)
    ASSERT(ESPChipStatusFlags.WIFIStatus == CONNECTED)
    if (ESPChipStatusFlags.TCPStatus == CONNECTED) {
        PRINT("There is already a TCP Connection open. Please close this one first")
        return false;
    }

    char cmd[100];
    strcpy(cmd, "AT+CIPSTART=\"TCP\",\"");
    strcat(cmd, target);
    strcat(cmd, "\",");
    strcat(cmd, port);

    if (ESP_SendCommand(cmd, "CONNECT", 2000)) {
        PRINT("Connected to %s at Port %s", target, port)
        ESPChipStatusFlags.TCPStatus = CONNECTED;
    } else {
        PRINT("Could not connect to %s at Port %s", target, port)
    }
    return ESPChipStatusFlags.TCPStatus;
}

bool ESP_TCP_Close(bool force) {
    if (!force) {
        ASSERT(ESPChipStatusFlags.ChipStatus)
        ASSERT(ESPChipStatusFlags.WIFIStatus == CONNECTED)
        if (ESPChipStatusFlags.TCPStatus == NOT_CONNECTED) {
            PRINT("No connection to close")
            return false;
        }
    }

    ESP_SendCommand("AT+CIPCLOSE", "OK", 1000);
    ESPChipStatusFlags.TCPStatus = NOT_CONNECTED;

    return ESPChipStatusFlags.TCPStatus;
}

bool ESP_TCP_SendData(char *data, int timeoutMs) {
    ASSERT(ESPChipStatusFlags.ChipStatus)
    ASSERT(ESPChipStatusFlags.WIFIStatus == CONNECTED)
    if (ESPChipStatusFlags.TCPStatus == NOT_CONNECTED) {
        PRINT("No open TCP connection to send to")
        return false;
    }

    char cmd[20];
    sprintf(cmd, "AT+CIPSEND=%d", strlen(data));

    // Clean the uart receive buffer before send the command
    ESP_CleanReceiveBuffer();
    // send the command and data
    if (!(ESP_SendCommand(cmd, ">", 100) && ESP_SendCommand(data, "SEND OK", 1000))) {
        PRINT("Failed to send data. Make sure everything is connected and try again later.")
    }

    // waiting for the response from the server
    bool responseArrived = false;
    TaskSleep(REFRESH_RESPOND_IN_MS / 2);
    for (int delay = 0; delay < timeoutMs; delay += REFRESH_RESPOND_IN_MS) {
        responseArrived = ESP_TCP_IsResponseAvailable();
        if (responseArrived)
            break;
        TaskSleep(REFRESH_RESPOND_IN_MS);
    }

    if (!responseArrived) {
        PRINT("Timeout: No response arrived!")
    } else if (uartToESP_ResponseArrived("CLOSED")) {
        ESPChipStatusFlags.TCPStatus = NOT_CONNECTED;
    }

    return responseArrived;
}

bool ESP_TCP_IsResponseAvailable() {
    return uartToESP_ResponseArrived("+IPD,");
}

char *ESP_TCP_GetResponse() {
    ASSERT(ESP_TCP_IsResponseAvailable())
    char *ret = ESP_TCP_CutResponseToBuffer();
    if (uartToESP_ResponseArrived("CLOSED")) {
        ESPChipStatusFlags.TCPStatus = NOT_CONNECTED;
    }
    return ret;
}

static char *ESP_TCP_CutResponseToBuffer() {
    char *responseBuf;
    char metaDataBuf[20];
    char data_length[20];
    // look for the response command in the uart responseBuffer
    // format: +IPD,5:Hallo
    uartToESP_Read("+IPD,", metaDataBuf, 20);
    char *endOfLength = strstr(metaDataBuf, ":");
    if (endOfLength == NULL || endOfLength - metaDataBuf > 19) {
        PRINT("Response too long")
        return 0;
    }
    // copy the length of the data to a new buffer and convert to int
    strncpy(data_length, metaDataBuf, endOfLength - metaDataBuf);
    data_length[endOfLength - metaDataBuf] = 0;
    int len = atoi(data_length);
    char startAtString[20];
    strcpy(startAtString, "+IPD,");
    strcat(startAtString, data_length);
    strcat(startAtString, ":");
    PRINT("Got %d bytes", len)

    // allocate memory for the response data and copy
    responseBuf = malloc(len + 1);
    uartToESP_Cut(startAtString, responseBuf, len);
    return responseBuf;
}
