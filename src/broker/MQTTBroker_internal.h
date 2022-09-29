#ifndef ENV5_MQTTBROKER_INTERNAL_HEADER
#define ENV5_MQTTBROKER_INTERNAL_HEADER

#include "Posting.h"
#include "Subscriber.h"

typedef struct {
    char *topic;
    subscriber_t subscriber;
} Subscription;

static void setBrokerDomain(char *ID);

static mqtt_errorCode setUserConfiguration(char *clientId, char *userId, char *password);

static char *concatDomainAndClientWithTopic(const char *topic);

static char *concatDomainWithTopic(const char *topic);

static void getTopic(posting_t *posting, const char *startOfTopic, int lengthOfTopic);

static int getNumberOfDataBytes(const char *startOfNumber, const char *endOfNumber);

static void getData(posting_t *posting, const char *startOfData, int dataLength);

static bool handleResponse(posting_t *posting, char *response);

#endif /* ENV5_MQTTBROKER_INTERNAL_HEADER */
