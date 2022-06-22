#define SOURCE_FILE "NETWORK"

#include "Network.h"
#include "common.h"
#include "esp.h"
#include <malloc.h>
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
    char *cmd = malloc(14 + strlen(credentials.ssid) + strlen(credentials.password));
    strcpy(cmd, "AT+CWJAP=\"");
    strcat(cmd, credentials.ssid);
    strcat(cmd, "\",\"");
    strcat(cmd, credentials.password);
    strcat(cmd, "\"");

    if (ESP_SendCommand(cmd, "WIFI GOT IP", 2500)) {
        PRINT("Connected to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = CONNECTED;
    } else {
        PRINT("Failed to connect to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = NOT_CONNECTED;
    }
    free(cmd);
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
