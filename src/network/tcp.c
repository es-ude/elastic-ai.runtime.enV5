#define SOURCE_FILE "TCP"

#include <string.h>

#include "tcp.h"
#include "Network.h"

#include "common.h"
#include "esp.h"
#include "TaskWrapper.h"

char TCP_response[100];

void TCP_Closed(void) {
    NetworkStatus.TCPStatus = NOT_CONNECTED;
}

bool TCP_Open(char *target, int port) {
    if (NetworkStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return false;
    }
    if (NetworkStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT("No WIFI connection!")
        return false;
    }
    if (NetworkStatus.TCPStatus == CONNECTED) {
        PRINT("There is already a TCP Connection open. Please close this one first")
        return false;
    }

    char port_buf[10];
    sprintf(port_buf, "%d", port);

    char cmd[100];
    strcpy(cmd, "AT+CIPSTART=\"TCP\",\"");
    strcat(cmd, target);
    strcat(cmd, "\",");
    strcat(cmd, port_buf);

    if (ESP_SendCommand(cmd, "CONNECT", 2000)) {
        PRINT("Connected to %s at Port %s", target, port_buf)
        NetworkStatus.TCPStatus = CONNECTED;
        TaskSleep(1000);
    } else {
        PRINT("Could not connect to %s at Port %s", target, port_buf)
    }
    return NetworkStatus.TCPStatus;
}

bool TCP_Close(bool force) {
    if (!force) {
        ASSERT(NetworkStatus.ChipStatus)
        ASSERT(NetworkStatus.WIFIStatus == CONNECTED)
        if (NetworkStatus.TCPStatus == NOT_CONNECTED) {
            PRINT("No connection to close")
            return false;
        }
    }

    ESP_SendCommand("AT+CIPCLOSE", "OK", 1000);
    NetworkStatus.TCPStatus = NOT_CONNECTED;

    return NetworkStatus.TCPStatus;
}

bool TCP_SendData(char *data, int timeoutMs) {
    if (NetworkStatus.TCPStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return false;
    }
    if (NetworkStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT("No WIFI connection!")
        return false;
    }
    if (NetworkStatus.TCPStatus == NOT_CONNECTED) {
        PRINT("No open TCP connection to send to")
        return false;
    }

    char cmd[20];
    sprintf(cmd, "AT+CIPSEND=%d", strlen(data) + 2); // +2 for /r/n which will be appended

    if (!ESP_SendCommand(cmd, ">", timeoutMs)) {
        PRINT_DEBUG("Not found >")
        return false;
    }

    bool responseArrived = ESP_SendCommandAndGetResponse(data, "+IPD,", timeoutMs, TCP_response);

    return responseArrived;
}

char *TCP_GetResponse(void) {
    return TCP_response;
}
