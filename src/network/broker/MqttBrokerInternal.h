#ifndef ENV5_MQTTBROKER_INTERNAL_HEADER
#define ENV5_MQTTBROKER_INTERNAL_HEADER

#include "Posting.h"
#include "Subscriber.h"

typedef struct mqttBrokerSubscription {
    char *topic;
    subscriber_t subscriber;
    struct mqttBrokerSubscription *next;
} mqttBrokerSubscription_t;

static void mqttBrokerInternalSetBrokerDomain(char *ID);

static void mqttBrokerInternalSetUserConfiguration(char *clientId, char *userId, char *password);

static void mqttBrokerInternalSetConnectionConfiguration(void);

static char *mqttBrokerInternalConcatDomainAndClientWithTopic(const char *topic);

static char *mqttBrokerInternalConcatDomainWithTopic(const char *topic);

static void mqttBrokerInternalGetTopic(posting_t *posting, const char *startOfTopic,
                                       int lengthOfTopic);

static int mqttBrokerInternalGetNumberOfDataBytes(const char *startOfNumber,
                                                  const char *endOfNumber);

static void mqttBrokerInternalGetData(posting_t *posting, const char *startOfData, int dataLength);

static bool mqttBrokerInternalHandleResponse(posting_t *posting, char *response);

#endif /* ENV5_MQTTBROKER_INTERNAL_HEADER */
