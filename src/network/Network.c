#include "Network.h"
#include "esp/espBase.h"
#include "esp/espTCP.h"
#include "stdio.h"

NetworkStatusFlags flags;

NetworkStatusFlags Network_GetStatusFlags() {
    ESP_StatusFlags espFlags = ESP_GetStatusFlags();
    flags.WIFIStatus = espFlags.WIFIStatus;
    flags.TCPStatus = espFlags.TCPStatus;
    flags.MQTTStatus = espFlags.MQTTStatus;
    return flags;
}

bool Network_Init(bool softInit) {
    return ESP_Init(softInit);
}

bool Network_ConnectToNetwork(NetworkCredentials credentials) {
    ESP_NetworkCredentials esp_credentials;
    esp_credentials.ssid = credentials.ssid;
    esp_credentials.password = credentials.password;
    return ESP_ConnectToNetwork(esp_credentials);
}

bool Network_DisconnectFromNetwork() {
    return ESP_DisconnectFromNetwork();
}

bool Network_TCP_Open(char *target, int port) {
    char buf[10];
    sprintf(buf, "%d", port);
    return ESP_TCP_Open(target, buf);
}

bool Network_TCP_Close(bool force) {
    return ESP_TCP_Close(force);
}

bool Network_TCP_SendData(char *data, int timeoutMs) {
    return ESP_TCP_SendData(data, timeoutMs);
}

bool Network_TCP_IsResponseAvailable() {
    return ESP_TCP_IsResponseAvailable();
}

char *Network_TCP_GetResponse() {
    return ESP_TCP_GetResponse();
}
