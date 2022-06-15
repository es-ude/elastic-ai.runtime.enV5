#define SOURCE_FILE "NETWORK"

#include "Network.h"
#include "common.h"
#include "esp.h"
#include <string.h>

void Network_ConnectToNetworkUntilConnected(NetworkCredentials_t credentials) {
    if (ESP_Status.WIFIStatus == CONNECTED)
        return;
    while (!Network_ConnectToNetwork(credentials));
}

bool Network_ConnectToNetwork(NetworkCredentials_t credentials) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return false;
    }
    if (ESP_Status.WIFIStatus == CONNECTED) {
        PRINT("Already connected to Network!")
        return true;
    }
    // if the length of the ssid + password cannot be longer than about 90 chars
    char cmd[100];
    strcpy(cmd, "AT+CWJAP=\"");
    strcat(cmd, credentials.ssid);
    strcat(cmd, "\",\"");
    strcat(cmd, credentials.password);
    strcat(cmd, "\"");

    if (ESP_SendCommand(cmd, "WIFI GOT IP", 1000)) {
        PRINT("Connected to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = CONNECTED;
    } else {
        PRINT("Failed to connect to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = NOT_CONNECTED;
    }
    return ESP_Status.WIFIStatus;
}

void Network_DisconnectFromNetwork(void) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT("No connection to disconnect from!")
        return;
    }

    if (ESP_SendCommand("AT+CWQAP", "OK", 5000)) {
        PRINT_DEBUG("Disconnected from Network")
    } else {
        PRINT("Failed to disconnect from Network")
    }
    ESP_Status.WIFIStatus = NOT_CONNECTED;
}
