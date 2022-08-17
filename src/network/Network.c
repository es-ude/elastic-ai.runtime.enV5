#define SOURCE_FILE "NETWORK"

#include "Network.h"
#include "at_commands.h"
#include "common.h"
#include "esp.h"
#include <stdlib.h>
#include <string.h>

void network_ConnectToNetworkUntilConnected(NetworkCredentials_t credentials) {
    if (ESP_Status.WIFIStatus == CONNECTED) {
        return;
    }
    while (!network_ConnectToNetwork(credentials)) {
        ;
    }
}

bool network_ConnectToNetwork(NetworkCredentials_t credentials) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return false;
    }
    if (ESP_Status.WIFIStatus == CONNECTED) {
        PRINT("Already connected to Network!")
        return true;
    }
    /* generate connect command with SSID and Password  from configuration.h*/
    size_t lengthOfString =
        AT_CONNECT_TO_NETWORK_LENGTH + strlen(credentials.ssid) + strlen(credentials.password);
    char *connectToNetwork = malloc(lengthOfString);
    snprintf(connectToNetwork, lengthOfString, AT_CONNECT_TO_NETWORK, credentials.ssid,
             credentials.password);

    if (esp_SendCommand(connectToNetwork, AT_CONNECT_TO_NETWORK_RESPONSE, 120000)) {
        PRINT("Connected to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = CONNECTED;
    } else {
        PRINT("Failed to connect to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = NOT_CONNECTED;
    }
    free(connectToNetwork);
    return ESP_Status.WIFIStatus;
}

void network_DisconnectFromNetwork(void) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT("No connection to disconnect from!")
        return;
    }

    char *disconnect = malloc(AT_DISCONNECT_LENGTH);
    strcpy(disconnect, AT_DISCONNECT);

    if (esp_SendCommand(disconnect, AT_DISCONNECT_RESPONSE, 5000)) {
        PRINT_DEBUG("Disconnected from Network")
    } else {
        PRINT("Failed to disconnect from Network")
    }
    ESP_Status.WIFIStatus = NOT_CONNECTED;

    free(disconnect);
}

void network_checkConnection(void) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!")
        return;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT("No connection to disconnect from!")
        return;
    }

    char *checkConnection = malloc(AT_CHECK_CONNECTION_LENGTH);
    strcpy(checkConnection, AT_CHECK_CONNECTION);

    esp_SendCommand(checkConnection, AT_CHECK_CONNCETION_RESPONSE, 5000);

    free(checkConnection);
}
