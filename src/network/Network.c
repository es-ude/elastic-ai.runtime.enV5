//
// Created by Lukas on 13.12.2021.
//

#include "Network.h"
#include "esp/espBase.h"
#include "esp/espTCP.h"
#include "esp/espMQTT.h"
#include "stdio.h"

NetworkStatusFlags flags;
NetworkStatusFlags Network_GetStatusFlags() {
    ESP_StatusFlags espFlags = ESP_GetStatusFlags();
    flags.WIFIStatus         = espFlags.WIFIStatus;
    flags.TCPStatus          = espFlags.TCPStatus;
    flags.MQTTStatus         = espFlags.MQTTStatus;
    return flags;
}

bool Network_Init(bool softInit) {
    return ESP_Init(softInit);
}

bool Network_ConnectToNetwork(NetworkCredentials credentials) {
    ESP_NetworkCredentials creds;
    creds.ssid     = credentials.ssid;
    creds.password = credentials.password;
    return ESP_ConnectToNetwork(creds);
}
bool Network_DisconnectFromNetwork() {
    return ESP_DisconnectFromNetwork();
}

bool Network_Ping(char *ipAdress) {
    return ESP_Ping(ipAdress);
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

bool Network_MQTT_SetClientId(char *clientId) {
    return ESP_MQTT_SetClientId(clientId);
}
bool Network_MQTT_Open(char *host, int port) {
    char buf[10];
    sprintf(buf, "%d", port);
    return ESP_MQTT_ConnectToBroker(host,buf);
}
bool Network_MQTT_Close(bool force) {
    return ESP_MQTT_Disconnect(force);
}
bool Network_MQTT_Publish(MQTT_Message message) {
    ESP_MQTT_Message msg={
        .topic=message.topic,
        .data=message.data
    };
    return ESP_MQTT_Publish(msg);
}
bool Network_MQTT_Subscribe(char *topic){
    return ESP_MQTT_Subscribe(topic);
}
bool Network_MQTT_Unsubscribe(char *topic){
    return ESP_MQTT_Unsubscribe(topic);
}
bool Network_MQTT_IsResponseAvailable(){
    return ESP_MQTT_IsResponseAvailable();
}
bool Network_MQTT_GetResponse(MQTT_Message *response) {
    ESP_MQTT_Message msg = {
            .data=NULL,
            .topic=NULL
    };
    bool ret = ESP_MQTT_GetResponse(&msg);
    response->topic = msg.topic;
    response->data = msg.data;
    return ret;
}
