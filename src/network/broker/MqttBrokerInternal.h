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

/*!
 * @important make sure you free the allocated ram!
 */
static char *mqttBrokerInternalConcatDomainAndClientWithTopic(const char *topic);

/*!
 * @important make sure you free the allocated ram!
 */
static char *mqttBrokerInternalConcatDomainWithTopic(const char *topic);

/*!
 * @important make sure you free the allocated ram!
 */
static char *mqttBrokerInternalGetTopic(const char *startOfTopic, size_t lengthOfTopic);

static size_t mqttBrokerInternalGetNumberOfDataBytes(const char *startOfNumber,
                                                     size_t lengthOfNumber);

static char *mqttBrokerInternalGetData(const char *startOfData, size_t dataLength);

static bool mqttBrokerInternalHandleResponse(posting_t *posting, char *response);

static void publishShortMessage(posting_t posting);

static void publishLongMessage(posting_t posting);

#endif /* ENV5_MQTTBROKER_INTERNAL_HEADER */
