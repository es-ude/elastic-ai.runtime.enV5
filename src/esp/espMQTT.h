#ifndef SENSOR_BOARD_ESPMQTT_H
#define SENSOR_BOARD_ESPMQTT_H
#define MAX_TOPIC_NAME_LENGTH 20

#include <stdbool.h>

typedef struct {
    char *topic;
    char *data;
} ESP_MQTT_Message;

bool ESP_MQTT_SetClientId(char *clientId);
bool ESP_MQTT_ConnectToBroker(char *host, char *port);
bool ESP_MQTT_Disconnect(bool force);
bool ESP_MQTT_Publish(ESP_MQTT_Message message);
bool ESP_MQTT_Subscribe(char *topic);
bool ESP_MQTT_Unsubscribe(char *topic);
bool ESP_MQTT_IsResponseAvailable();
bool ESP_MQTT_GetResponse(ESP_MQTT_Message *response);

#endif //SENSOR_BOARD_ESPMQTT_H
