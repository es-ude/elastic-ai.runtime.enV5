#define SOURCE_FILE "MQTT-BROKER"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "AtCommands.h"
#include "CException.h"
#include "Common.h"
#include "Esp.h"
#include "MqttBroker.h"
#include "MqttBrokerInternal.h"

// header from elastic-ai.runtime.c
#include "CommunicationEndpoint.h"
#include "Protocol.h"
#include "Subscriber.h"
#include "TopicMatcher.h"

/* region VARIABLES */

char *mqttBrokerBrokerId = NULL;
char *mqttBrokerClientId = NULL;
mqttBrokerSubscription_t *subscriptions = NULL;

extern mqttBrokerHost_t mqttHost;

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

void mqttBrokerConnectToBrokerUntilSuccessful(char *brokerDomain, char *clientID) {
    while (espStatus.MQTTStatus == NOT_CONNECTED) {
        CEXCEPTION_T exception_mqttBrokerConnectToBroker;
        Try {
            mqttBrokerConnectToBroker(brokerDomain, clientID);
            PRINT_DEBUG("Connected");
        }
        Catch(exception_mqttBrokerConnectToBroker) {
            if (exception_mqttBrokerConnectToBroker == MQTT_CONNECTION_FAILED ||
                exception_mqttBrokerConnectToBroker == MQTT_ESP_WRONG_ANSWER) {
                PRINT_DEBUG("Connection failed. Trying again now!");
            } else {
                Throw(exception_mqttBrokerConnectToBroker);
            }
        }
    }
}

void mqttBrokerConnectToBroker(char *brokerDomain, char *clientID) {
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT_DEBUG("Could not connect to MQTT broker! Chip problem.");
        Throw(MQTT_ESP_CHIP_FAILED);
    }
    if (espStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT_DEBUG("Could not connect to MQTT broker! No Wifi connection.");
        Throw(MQTT_WIFI_FAILED);
    }
    if (espStatus.MQTTStatus == CONNECTED) {
        PRINT_DEBUG("MQTT Broker already connected! Disconnect first");
        Throw(MQTT_ALREADY_CONNECTED);
    }

    mqttBrokerInternalSetUserConfiguration(clientID, mqttHost.userID, mqttHost.password);
    mqttBrokerInternalSetBrokerDomain(brokerDomain);
    mqttBrokerInternalSetConnectionConfiguration();

    size_t commandLength =
        AT_MQTT_CONNECT_TO_BROKER_LENGTH + strlen(mqttHost.ip) + strlen(mqttHost.port);
    char connectToBroker[commandLength];
    snprintf(connectToBroker, commandLength, AT_MQTT_CONNECT_TO_BROKER, mqttHost.ip, mqttHost.port);

    espErrorCode_t error =
        espSendCommand(connectToBroker, AT_MQTT_CONNECT_TO_BROKER_RESPONSE, 60000);
    switch (error) {
    case ESP_NO_ERROR:
        PRINT_DEBUG("Connected to %s at Port %s", mqttHost.ip, mqttHost.port);
        espStatus.MQTTStatus = CONNECTED;
        espSetMqttReceiverFunction(mqttBrokerReceive);
        break;
    case ESP_WRONG_ANSWER_RECEIVED:
        PRINT_DEBUG("Could not connect to %s at Port %s. Wrong answer!", mqttHost.ip,
                    mqttHost.port);
        Throw(MQTT_ESP_WRONG_ANSWER);
    case ESP_UART_IS_BUSY:
        PRINT_DEBUG("Could not connect to %s at Port %s. UART busy!", mqttHost.ip, mqttHost.port);
        Throw(MQTT_ESP_CHIP_FAILED);
    default:
        PRINT_DEBUG("Could not connect to %s at Port %s", mqttHost.ip, mqttHost.port);
        Throw(MQTT_CONNECTION_FAILED);
    }
}

void mqttBrokerDisconnect(bool force) {
    if (!force && espStatus.ChipStatus == CONNECTED && espStatus.WIFIStatus == CONNECTED &&
        espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT_DEBUG("No connection to close!");
        return;
    }

    char disconnect[AT_MQTT_DISCONNECT_FROM_BROKER_LENGTH];
    strcpy(disconnect, AT_MQTT_DISCONNECT_FROM_BROKER);

    if (ESP_NO_ERROR == espSendCommand(disconnect, AT_MQTT_DISCONNECT_FROM_BROKER_RESPONSE, 5000)) {
        espStatus.MQTTStatus = NOT_CONNECTED;

        free(mqttBrokerBrokerId);
        mqttBrokerBrokerId = NULL;

        free(mqttBrokerClientId);
        mqttBrokerClientId = NULL;

        PRINT_DEBUG("MQTT Broker disconnected!");
    } else {
        Throw(MQTT_COULD_NOT_DISCONNECT_BROKER);
    }
}

void mqttBrokerReceive(char *response) {
    posting_t posting = {0};
    if (mqttBrokerInternalHandleResponse(&posting, response)) {
        mqttBrokerSubscription_t *current = subscriptions;
        while (current != NULL) {
            if (topicMatcherCheckIfTopicMatches(current->topic, posting.topic)) {
                current->subscriber.deliver(posting);
                return;
            }
            current = current->next;
        }

        free(posting.topic);
        free(posting.data);
    }
}

/* endregion */

/* region communicationEndpoint.h */

void communicationEndpointPublish(posting_t posting) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!");
        return;
    }
    posting_t post = {.data = posting.data,
                      .retain = posting.retain,
                      .topic = mqttBrokerInternalConcatDomainAndClientWithTopic(posting.topic)};
    communicationEndpointPublishRaw(post);
    free(post.topic);
}

void communicationEndpointPublishRemote(posting_t posting) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!");
        return;
    }
    posting_t post = {.data = posting.data,
                      .retain = posting.retain,
                      .topic = mqttBrokerInternalConcatDomainWithTopic(posting.topic)};
    communicationEndpointPublishRaw(post);
    free(post.topic);
}

void communicationEndpointPublishRaw(posting_t posting) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!");
        return;
    }

    size_t commandLength = AT_MQTT_PUBLISH_LENGTH + strlen(posting.topic) + strlen(posting.data);

    if (commandLength >= 256) {
        publishLongMessage(posting);
    } else {
        publishShortMessage(posting);
    }
}

void communicationEndpointSubscribe(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic);
        return;
    }

    char *fullTopic = mqttBrokerInternalConcatDomainAndClientWithTopic(topic);
    communicationEndpointSubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void communicationEndpointSubscribeRemote(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic);
        return;
    }

    char *fullTopic = mqttBrokerInternalConcatDomainWithTopic(topic);
    communicationEndpointSubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void communicationEndpointSubscribeRaw(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic);
        return;
    }

    size_t commandLength = AT_MQTT_SUBSCRIBE_TOPIC_LENGTH + strlen(topic);
    char subscribeTopic[commandLength];
    snprintf(subscribeTopic, commandLength, AT_MQTT_SUBSCRIBE_TOPIC, topic);

    if (ESP_NO_ERROR != espSendCommand(subscribeTopic, AT_MQTT_SUBSCRIBE_TOPIC_RESPONSE, 5000)) {
        PRINT("Could not subscribe to topic: %s. Have You already subscribed?", topic);
    } else {
        mqttBrokerSubscription_t *newSubscription = calloc(1, sizeof(mqttBrokerSubscription_t));
        newSubscription->topic = calloc(strlen(topic) + 1, sizeof(char));
        strcpy(newSubscription->topic, topic);
        newSubscription->subscriber = subscriber;

        if (subscriptions != NULL) {
            mqttBrokerSubscription_t *current = subscriptions;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newSubscription;
        } else {
            subscriptions = newSubscription;
        }
        PRINT("Subscribed to %s", topic);
    }
}

void communicationEndpointUnsubscribe(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic);
        return;
    }

    char *fullTopic = mqttBrokerInternalConcatDomainAndClientWithTopic(topic);
    communicationEndpointUnsubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void communicationEndpointUnsubscribeRemote(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic);
        return;
    }

    char *fullTopic = mqttBrokerInternalConcatDomainWithTopic(topic);
    communicationEndpointUnsubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void communicationEndpointUnsubscribeRaw(char *topic,
                                         __attribute__((unused)) subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic);
        return;
    }

    size_t commandLength = AT_MQTT_UNSUBSCRIBE_TOPIC_LENGTH + strlen(topic);
    char command[commandLength];
    snprintf(command, commandLength, AT_MQTT_UNSUBSCRIBE_TOPIC, topic);

    if (ESP_NO_ERROR != espSendCommand(command, AT_MQTT_UNSUBSCRIBE_TOPIC_RESPONSE, 5000)) {
        PRINT("Could not unsubscribe from topic %s. Have you subscribed beforehand?", topic);
    } else {
        if (subscriptions != NULL) {
            mqttBrokerSubscription_t *current = subscriptions;
            mqttBrokerSubscription_t *last = NULL;
            while (current != NULL) {
                if (topicMatcherCheckIfTopicMatches(current->topic, topic)) {
                    if (last != NULL) {
                        last->next = current->next;
                    } else {
                        subscriptions = current->next;
                    }
                    free(current->topic);
                    free(current);
                    break;
                }
                last = current;
                current = current->next;
            }
        }

        PRINT("Unsubscribed from %s.", topic);
    }
}

char *communicationEndpointGetBrokerDomain() {
    return mqttBrokerBrokerId;
}

char *communicationEndpointGetClientId() {
    return mqttBrokerClientId;
}

void publishAliveStatusMessageWithMandatoryAttributes(status_t status) {
    status.type = "enV5";
    status.id = mqttBrokerClientId;
    status.state = STATUS_STATE_ONLINE;
    protocolPublishStatus(status);
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static void mqttBrokerInternalSetBrokerDomain(char *ID) {
    if (mqttBrokerBrokerId != NULL) {
        free(mqttBrokerBrokerId);
    }

    mqttBrokerBrokerId = calloc(strlen(ID) + 1, sizeof(char));
    strcpy(mqttBrokerBrokerId, ID);
}

static void mqttBrokerInternalSetUserConfiguration(char *clientId, char *userId, char *password) {
    if (mqttBrokerClientId != NULL) {
        free(mqttBrokerClientId);
        mqttBrokerClientId = NULL;
    }
    mqttBrokerClientId = calloc(strlen(clientId) + 1, sizeof(char));
    strcpy(mqttBrokerClientId, clientId);

    size_t commandLength =
        AT_MQTT_USER_CONFIGURATION_LENGTH + strlen(clientId) + strlen(userId) + strlen(password);
    char setClientID[commandLength];
    snprintf(setClientID, commandLength, AT_MQTT_USER_CONFIGURATION, clientId, userId, password);

    switch (espSendCommand(setClientID, AT_MQTT_USER_CONFIGURATION_RESPONSE, 1000)) {
    case ESP_NO_ERROR:
        PRINT("Set client id to %s", clientId);
        break;
    case ESP_WRONG_ANSWER_RECEIVED:
        PRINT("Could not set client id to %s, aborting...", clientId);
        Throw(MQTT_ESP_WRONG_ANSWER);
    default:
        PRINT("Could not set client id to %s, aborting...", clientId);
        Throw(MQTT_ESP_CHIP_FAILED);
    }
}

static void mqttBrokerInternalSetConnectionConfiguration(void) {
    char *lwt_topic = mqttBrokerInternalConcatDomainAndClientWithTopic(STATUS);
    size_t lwt_topic_length = strlen(lwt_topic);

    char *lwt_message = getStatusMessage(
        (status_t){.id = mqttBrokerClientId, .state = STATUS_STATE_OFFLINE, .type = "enV5"});
    size_t lwt_message_length = strlen(lwt_message);

    size_t commandLength =
        AT_MQTT_CONNECTION_CONFIGURATION_LENGTH + lwt_topic_length + lwt_message_length;
    char setConnectionConfiguration[commandLength];
    snprintf(setConnectionConfiguration, commandLength, AT_MQTT_CONNECTION_CONFIGURATION, lwt_topic,
             lwt_message);

    free(lwt_topic);
    free(lwt_message);

    switch (espSendCommand(setConnectionConfiguration, AT_MQTT_CONNECTION_CONFIGURATION_RESPONSE,
                           5000)) {
    case ESP_NO_ERROR:
        PRINT("Set connection configuration successful.");
        break;
    case ESP_WRONG_ANSWER_RECEIVED:
        PRINT("Failed to store Connection settings! Wrong answer!");
        Throw(MQTT_ESP_WRONG_ANSWER);
    case ESP_UART_IS_BUSY:
        PRINT("Failed to store Connection settings! UART busy!");
        Throw(MQTT_ESP_CHIP_FAILED);
    default:
        PRINT("Failed to store Connection settings!");
        Throw(MQTT_CONNECTION_FAILED);
    }
}

static char *mqttBrokerInternalConcatDomainAndClientWithTopic(const char *topic) {
    size_t lengthOfResult =
        3 + strlen(mqttBrokerBrokerId) + strlen(mqttBrokerClientId) + strlen(topic);
    char *result = calloc(lengthOfResult, sizeof(char));
    snprintf(result, lengthOfResult, "%s/%s/%s", mqttBrokerBrokerId, mqttBrokerClientId, topic);
    return result;
}

static char *mqttBrokerInternalConcatDomainWithTopic(const char *topic) {
    size_t lengthOfResult = 2 + strlen(mqttBrokerBrokerId) + strlen(topic);
    char *result = calloc(lengthOfResult, sizeof(char));
    snprintf(result, lengthOfResult, "%s/%s", mqttBrokerBrokerId, topic);
    return result;
}

static char *mqttBrokerInternalGetTopic(const char *startOfTopic, size_t lengthOfTopic) {
    char *topic = calloc(lengthOfTopic + 1, sizeof(char));
    memcpy(topic, startOfTopic, lengthOfTopic);
    return topic;
}

static size_t mqttBrokerInternalGetNumberOfDataBytes(const char *startOfNumber,
                                                     size_t lengthOfNumber) {
    char numberAsString[lengthOfNumber + 1];
    memcpy(numberAsString, startOfNumber, lengthOfNumber);
    return strtol(numberAsString, NULL, 10);
}

static char *mqttBrokerInternalGetData(const char *startOfData, size_t dataLength) {
    char *dataBuffer = calloc(dataLength + 1, sizeof(char));
    memcpy(dataBuffer, startOfData, dataLength);
    return dataBuffer;
}

static bool mqttBrokerInternalHandleResponse(posting_t *posting, char *response) {
    if (strlen(response) == 0) {
        PRINT_DEBUG("Empty Response.");
        free(response);
        return false;
    }

    /* region TOPIC */
    char *startOfTopic = strstr(response, ",\"") + 2;
    char *endOfTopic = strstr(startOfTopic, "\",");
    posting->topic = mqttBrokerInternalGetTopic(startOfTopic, endOfTopic - startOfTopic);
    PRINT_DEBUG("Got topic: %s", posting->topic);
    /* endregion TOPIC */

    /* region DATA LENGTH */
    char *startOfDataLength = endOfTopic + 2;
    char *endOfDataLength = strstr(startOfDataLength, ",");
    size_t dataLength = mqttBrokerInternalGetNumberOfDataBytes(startOfDataLength,
                                                               endOfDataLength - startOfDataLength);
    PRINT_DEBUG("Got length of Data: %i", dataLength);
    /* endregion DATA LENGTH */

    /* region PAYLOAD */
    posting->data = mqttBrokerInternalGetData(endOfDataLength + 1, dataLength);
    PRINT_DEBUG("Got data: %s", posting->data);
    /* endregion PAYLOAD */

    return true;
}

static void publishShortMessage(posting_t posting) {
    size_t commandLength = AT_MQTT_PUBLISH_LENGTH + strlen(posting.topic) + strlen(posting.data);
    char publishData[commandLength];
    snprintf(publishData, commandLength, AT_MQTT_PUBLISH, posting.topic, posting.data,
             posting.retain ? "1" : "0");

    if (ESP_NO_ERROR != espSendCommand(publishData, AT_MQTT_PUBLISH_RESPONSE, 5000)) {
        PRINT("Could not publish to topic: %s", posting.topic);
    } else {
        PRINT("Published to %s", posting.topic);
    }
}

static void publishLongMessage(posting_t posting) {
    size_t dataStringLength = (size_t)(log10((double)(strlen(posting.data)))) + 1;

    size_t commandLength = AT_MQTT_PUBLISH_LONG_LENGTH + strlen(posting.topic) + dataStringLength;
    char publishData[commandLength];

    snprintf(publishData, commandLength, AT_MQTT_PUBLISH_LONG, posting.topic,
             (unsigned long)strlen(posting.data), posting.retain ? "1" : "0");

    if (ESP_NO_ERROR != espSendCommand(publishData, AT_MQTT_PUBLISH_LONG_START, 60000)) {
        PRINT("Could not publish to topic: %s", posting.topic);
    } else {
        if (ESP_NO_ERROR != espSendCommand(posting.data, AT_MQTT_PUBLISH_LONG_RESPONSE, 60000)) {
            PRINT("Problems when publishing: %s", posting.topic);
        } else {
            PRINT("Published to %s", posting.topic);
        }
    }
}

/* endregion */
