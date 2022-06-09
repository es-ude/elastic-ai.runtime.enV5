#ifndef ENV5_MQTTBROKER_INTERNAL_H
#define ENV5_MQTTBROKER_INTERNAL_H

#include "subscriber.h"
#include "posting.h"

typedef struct Subscription {
    char *topic;
    Subscriber subscriber;
} Subscription;

bool MQTT_Broker_HandleResponse(Posting *posting, char *response);

char *MQTT_Broker_concatIDWithTopic(const char *topic);

#endif //ENV5_MQTTBROKER_INTERNAL_H
