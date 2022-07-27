#define SOURCE_FILE "NETWORK"

#include "Network.h"
#include "common.h"
#include "esp.h"
#include <stdlib.h>
#include <string.h>

void Network_ConnectToNetworkUntilConnected(NetworkCredentials_t credentials) {
    if (ESP_Status.WIFIStatus == CONNECTED)
        return;
    while (!Network_ConnectToNetwork(credentials))
        ;
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
    /* generate connect command with SSID and Password  from configuration.h*/
    size_t lengthOfString = 14 + strlen(credentials.ssid) + strlen(credentials.password);
    char *connectToNetwork = malloc(lengthOfString);
    snprintf(connectToNetwork, lengthOfString, "AT+CWJAP\"%s\",\"%s\"", credentials.ssid,
             credentials.password);

    if (ESP_SendCommand(connectToNetwork, "WIFI GOT IP", 2500)) {
        PRINT("Connected to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = CONNECTED;
    } else {
        PRINT("Failed to connect to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = NOT_CONNECTED;
    }
    free(connectToNetwork);
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
