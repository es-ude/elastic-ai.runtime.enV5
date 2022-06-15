#ifndef ENV5_MQTTBROKER_INTERNAL_H
#define ENV5_MQTTBROKER_INTERNAL_H

#include "posting.h"
#include "subscriber.h"

typedef struct Subscription {
    char *topic;
    Subscriber subscriber;
} Subscription;

void MQTT_Broker_getTopic(Posting *posting, const char *start,
                          int lengthOfTopic);

int MQTT_Broker_getDataLength(char *start, const char *end);

void MQTT_Broker_getData(Posting *posting, const char *end, int dataLength);

bool MQTT_Broker_HandleResponse(Posting *posting, char *response);

char *MQTT_Broker_concatIDWithTopic(const char *topic);

#endif // ENV5_MQTTBROKER_INTERNAL_H
