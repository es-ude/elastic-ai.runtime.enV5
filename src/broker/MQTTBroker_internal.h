#ifndef ENV5_MQTTBROKER_INTERNAL_HEADER
#define ENV5_MQTTBROKER_INTERNAL_HEADER

#include "posting.h"
#include "subscriber.h"

typedef struct {
    char *topic;
    Subscriber subscriber;
} Subscription;

static char *concatDomainAndClientWithTopic(const char *topic);

static char *concatDomainWithTopic(const char *topic);

static void getTopic(Posting *posting, const char *startOfTopic, int lengthOfTopic);

static int getNumberOfDataBytes(const char *startOfNumber, const char *endOfNumber);

static void getData(Posting *posting, const char *startOfData, int dataLength);

static bool handleResponse(Posting *posting, char *response);

#endif /* ENV5_MQTTBROKER_INTERNAL_HEADER */
