#define SOURCE_FILE "NETWORK"

#include "Network.h"
#include "common.h"
#include "esp.h"
#include "TaskWrapper.h"
#include "uartToESP.h"
#include <string.h>

NetworkStatus_t NetworkStatus = {
        .ChipStatus = ESP_CHIP_NOT_OK,
        .WIFIStatus = NOT_CONNECTED,
        .TCPStatus = NOT_CONNECTED,
        .MQTTStatus = NOT_CONNECTED};

bool Network_init(void) {
    // init the uart interface for at
    uartToEsp_Init(esp32UartDevice);

    ESP_SoftReset();

    while (!ESP_CheckIsResponding()) {
        PRINT("ESP ACK_CHECK failed")
        TaskSleep(1000);
        ESP_SoftReset();
    }

    PRINT("ESP is responding")
    // disable echo, make the output more clean
    ESP_SendCommand("ATE0", "OK", 100);
    // disable multiple connections
    int retriesLeft = 2;
    while (!ESP_SendCommand("AT+CIPMUX=0", "OK", 100) && retriesLeft > 0) {
        PRINT("could not set esp to single connection mode!")
        retriesLeft--;
    }
    NetworkStatus.ChipStatus = ESP_CHIP_OK;
    NetworkStatus.WIFIStatus = NOT_CONNECTED;
    NetworkStatus.TCPStatus = NOT_CONNECTED;
    NetworkStatus.MQTTStatus = NOT_CONNECTED;
    return true;
}

bool Network_ConnectToNetwork(NetworkCredentials credentials) {
    if (NetworkStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return false;
    }
    if (NetworkStatus.WIFIStatus == CONNECTED) {
        PRINT("Already connected to Network!")
        return false;
    }
    // if the length of the ssid + password cannot be longer than about 90 chars
    char cmd[100];
    strcpy(cmd, "AT+CWJAP=\"");
    strcat(cmd, credentials.ssid);
    strcat(cmd, "\",\"");
    strcat(cmd, credentials.password);
    strcat(cmd, "\"");

    if (ESP_SendCommand(cmd, "WIFI GOT IP", 1000)) {
        PRINT("Connected to %s", credentials.ssid)
        NetworkStatus.WIFIStatus = CONNECTED;
    } else {
        PRINT("Failed to connect to Network: %s", credentials.ssid)
        NetworkStatus.WIFIStatus = NOT_CONNECTED;
    }
    return NetworkStatus.WIFIStatus;
}

void Network_ConnectToNetworkPlain(char *ssid, char *password) {
    Network_ConnectToNetwork((NetworkCredentials) {.ssid = ssid, .password=password});
}

void Network_DisconnectFromNetwork(void) {
    if (NetworkStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return;
    }
    if (NetworkStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT("No connection to disconnect from!")
        return;
    }

    ESP_SendCommand("AT+CWQAP", "OK", 5000);
    NetworkStatus.WIFIStatus = NOT_CONNECTED;
}
