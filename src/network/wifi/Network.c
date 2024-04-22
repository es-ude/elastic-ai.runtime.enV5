#define SOURCE_FILE "NETWORK"

#include <stdlib.h>
#include <string.h>

#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include "FreeRtosTaskWrapper.h"
#include "HTTP.h"
#include "Network.h"

extern networkCredentials_t networkCredentials;

networkErrorCode_t networkTryToConnectToNetworkUntilSuccessful() {
    if (espStatus.WIFIStatus == CONNECTED) {
        PRINT_DEBUG("Already connected to Network! Disconnect first.");
        return NETWORK_WIFI_ALREADY_CONNECTED;
    }

    while (espStatus.WIFIStatus == NOT_CONNECTED) {
        networkErrorCode_t networkErrorCode = networkConnectToNetwork();
        if (networkErrorCode == NETWORK_NO_ERROR) {
            break;
        } else if (networkErrorCode == NETWORK_ESP_CHIP_FAILED) {
            PRINT("ESP not reachable. Abort try to connect!");
            return NETWORK_ESP_CHIP_FAILED;
        } else {
            freeRtosTaskWrapperTaskSleep(1000);
            PRINT_DEBUG("Connection failed. Trying again now!");
        }
    }

    freeRtosTaskWrapperTaskSleep(5000);

    return NETWORK_NO_ERROR;
}

networkErrorCode_t networkConnectToNetwork() {
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT_DEBUG("Chip not working! Can't connect to network.");
        return NETWORK_ESP_CHIP_FAILED;
    }
    if (espStatus.WIFIStatus == CONNECTED) {
        PRINT_DEBUG("Already connected to Network! Disconnect first.");
        return NETWORK_WIFI_ALREADY_CONNECTED;
    }

    // generate connect command with SSID and Password from configuration.h
    size_t lengthOfString = AT_CONNECT_TO_NETWORK_LENGTH + strlen(networkCredentials.ssid) +
                            strlen(networkCredentials.password);
    char *connectToNetwork = malloc(lengthOfString);
    snprintf(connectToNetwork, lengthOfString, AT_CONNECT_TO_NETWORK, networkCredentials.ssid,
             networkCredentials.password);

    // send connect request to ESP32
    espErrorCode_t espErrorCode =
        espSendCommand(connectToNetwork, AT_CONNECT_TO_NETWORK_RESPONSE, 120000);
    free(connectToNetwork);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT("Connected to Network: %s", networkCredentials.ssid);
        espStatus.WIFIStatus = CONNECTED;
        HTTPSetReceiverFunction(); // TODO: should this be called here??
        return NETWORK_NO_ERROR;
    } else {
        PRINT("Failed to connect to Network: %s", networkCredentials.ssid);
        espStatus.WIFIStatus = NOT_CONNECTED;
        return NETWORK_ESTABLISH_CONNECTION_FAILED;
    }
}

void networkDisconnectFromNetwork(void) {
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT_DEBUG("Chip not working!");
        return;
    }
    if (espStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT_DEBUG("No connection to disconnect from!");
        return;
    }

    char *disconnect = malloc(AT_DISCONNECT_LENGTH);
    strcpy(disconnect, AT_DISCONNECT);
    espErrorCode_t espErrorCode = espSendCommand(disconnect, AT_DISCONNECT_RESPONSE, 5000);
    free(disconnect);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT_DEBUG("Disconnected from Network");
        espStatus.WIFIStatus = NOT_CONNECTED;
        espStatus.MQTTStatus = NOT_CONNECTED;
    } else {
        PRINT("Failed to disconnect from Network");
    }
}

networkErrorCode_t networkCheckConnection(void) {
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Chip not working!");
        return NETWORK_ESP_CHIP_FAILED;
    }
    if (espStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT("No connection to disconnect from!");
        return NETWORK_ESTABLISH_CONNECTION_FAILED;
    }

    char *checkConnection = malloc(AT_CHECK_CONNECTION_LENGTH);
    strcpy(checkConnection, AT_CHECK_CONNECTION);
    espErrorCode_t espErrorCode =
        espSendCommand(checkConnection, AT_CHECK_CONNCETION_RESPONSE, 5000);
    free(checkConnection);

    if (espErrorCode == ESP_NO_ERROR) {
        return NETWORK_NO_ERROR;
    }
    return NETWORK_ESTABLISH_CONNECTION_FAILED;
}
