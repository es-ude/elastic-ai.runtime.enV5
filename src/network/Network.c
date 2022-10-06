#define SOURCE_FILE "NETWORK"

#include "Network.h"
#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include "FreeRtosTaskWrapper.h"
#include <stdlib.h>
#include <string.h>

network_errorCode network_TryToConnectToNetworkUntilSuccessful(NetworkCredentials_t credentials) {
    if (ESP_Status.WIFIStatus == CONNECTED) {
        PRINT_DEBUG("Already connected to Network! Disconnect first.")
        return NETWORK_WIFI_ALREADY_CONNECTED;
    }

    while (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        network_errorCode networkErrorCode = network_ConnectToNetwork(credentials);
        if (networkErrorCode == NETWORK_NO_ERROR) {
            break;
        } else if (networkErrorCode == NETWORK_ESP_CHIP_FAILED) {
            PRINT("ESP not reachable. Abort try to connect!")
            return NETWORK_ESP_CHIP_FAILED;
        } else {
            TaskSleep(1000);
            PRINT_DEBUG("Connection failed. Trying again now!")
        }
    }

    return NETWORK_NO_ERROR;
}

network_errorCode network_ConnectToNetwork(NetworkCredentials_t credentials) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT_DEBUG("Chip not working! Can't connect to network.")
        return NETWORK_ESP_CHIP_FAILED;
    }
    if (ESP_Status.WIFIStatus == CONNECTED) {
        PRINT_DEBUG("Already connected to Network! Disconnect first.")
        return NETWORK_WIFI_ALREADY_CONNECTED;
    }

    // generate connect command with SSID and Password  from configuration.h
    size_t lengthOfString =
        AT_CONNECT_TO_NETWORK_LENGTH + strlen(credentials.ssid) + strlen(credentials.password);
    char *connectToNetwork = malloc(lengthOfString);
    snprintf(connectToNetwork, lengthOfString, AT_CONNECT_TO_NETWORK, credentials.ssid,
             credentials.password);

    // send connect to network command
    esp_errorCode espErrorCode =
        esp_SendCommand(connectToNetwork, AT_CONNECT_TO_NETWORK_RESPONSE, 120000);
    free(connectToNetwork);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT("Connected to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = CONNECTED;
        return NETWORK_NO_ERROR;
    } else {
        PRINT("Failed to connect to Network: %s", credentials.ssid)
        ESP_Status.WIFIStatus = NOT_CONNECTED;
        return NETWORK_ESTABLISH_CONNECTION_FAILED;
    }
}

void network_DisconnectFromNetwork(void) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT_DEBUG("Chip not working!")
        return;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT_DEBUG("No connection to disconnect from!")
        return;
    }

    char *disconnect = malloc(AT_DISCONNECT_LENGTH);
    strcpy(disconnect, AT_DISCONNECT);
    esp_errorCode espErrorCode = esp_SendCommand(disconnect, AT_DISCONNECT_RESPONSE, 5000);
    free(disconnect);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT_DEBUG("Disconnected from Network")
        ESP_Status.WIFIStatus = NOT_CONNECTED;
        ESP_Status.MQTTStatus = NOT_CONNECTED;
    } else {
        PRINT("Failed to disconnect from Network")
    }
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
