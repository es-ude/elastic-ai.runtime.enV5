#define SOURCE_FILE "MQTT-BROKER"

#include <stdlib.h>
#include <string.h>

#include "AtCommands.h"
#include "Common.h"
#include "Esp.h"
#include "MqttBroker.h"
#include "MqttBrokerInternal.h"

// header from elastic-ai.runtime.c
#include "CommunicationEndpoint.h"
#include "Posting.h"
#include "Subscriber.h"
#include "TopicMatcher.h"

/* region VARIABLES */

char *mqttBrokerBrokerId = NULL;
char *mqttBrokerClientId = NULL;
uint8_t mqttBrokerNumberOfSubscriptions = 0;
mqttBrokerSubscription_t mqttBrokerSubscriptions[MAX_SUBSCRIBER];
bool mqttBrokerReceiverFunctionSet = false;

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

mqttBrokerErrorCode_t mqttBrokerConnectToBrokerUntilSuccessful(mqttBrokerHost_t mqttHost,
                                                               char *brokerDomain, char *clientID) {
    if (espStatus.MQTTStatus == CONNECTED) {
        PRINT("MQTT Broker already connected! Disconnect first")
        return MQTT_ALREADY_CONNECTED;
    }

    while (espStatus.MQTTStatus == NOT_CONNECTED) {
        mqttBrokerErrorCode_t mqttErrorCode =
            mqttBrokerConnectToBroker(mqttHost, brokerDomain, clientID);
        if (mqttErrorCode == MQTT_WIFI_FAILED) {
            PRINT("Could not connect to MQTT broker! No Wifi connection.")
            return MQTT_WIFI_FAILED;
        } else if (mqttErrorCode == MQTT_ESP_CHIP_FAILED) {
            PRINT("Could not connect to MQTT broker! Chip problem.")
            return MQTT_ESP_CHIP_FAILED;
        } else if (mqttErrorCode != MQTT_NO_ERROR) {
            PRINT_DEBUG("Connection failed. Trying again now!")
        }
    }

    return MQTT_NO_ERROR;
}

mqttBrokerErrorCode_t mqttBrokerConnectToBroker(mqttBrokerHost_t credentials, char *brokerDomain,
                                                char *clientID) {
    if (espStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Could not connect to MQTT broker! Chip problem.")
        return MQTT_ESP_CHIP_FAILED;
    }
    if (espStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT("Could not connect to MQTT broker! No Wifi connection.")
        return MQTT_WIFI_FAILED;
    }
    if (espStatus.MQTTStatus == CONNECTED) {
        PRINT("MQTT Broker already connected! Disconnect first")
        return MQTT_ALREADY_CONNECTED;
    }

    // store mqtt client/domain
    mqttBrokerErrorCode_t userConfigError =
        mqttBrokerInternalSetUserConfiguration(clientID, credentials.userID, credentials.password);
    if (userConfigError != MQTT_NO_ERROR) {
        return userConfigError;
    }
    mqttBrokerInternalSetBrokerDomain(brokerDomain);

    // store connection configuration
    mqttBrokerErrorCode_t connectionConfigError = mqttBrokerInternalSetConnectionConfiguration();
    if (connectionConfigError != MQTT_NO_ERROR) {
        return connectionConfigError;
    }

    // generate connect command with ip and port
    size_t commandLength =
        AT_MQTT_CONNECT_TO_BROKER_LENGTH + strlen(credentials.ip) + strlen(credentials.port);
    char *connectToBroker = malloc(commandLength);
    snprintf(connectToBroker, commandLength, AT_MQTT_CONNECT_TO_BROKER, credentials.ip,
             credentials.port);

    // send connect to broker command
    espErrorCode_t espErrorCode =
        espSendCommand(connectToBroker, AT_MQTT_CONNECT_TO_BROKER_RESPONSE, 60000);
    free(connectToBroker);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT("Connected to %s at Port %s", credentials.ip, credentials.port)
        espStatus.MQTTStatus = CONNECTED;
        if (!mqttBrokerReceiverFunctionSet) {
            espSetMqttReceiverFunction(mqttBrokerReceive);
            mqttBrokerReceiverFunctionSet = true;
        }
        publishAliveStatusMessage();
        return MQTT_NO_ERROR;
    } else if (espErrorCode == ESP_WRONG_ANSWER_RECEIVED) {
        PRINT("Could not connect to %s at Port %s. Wrong answer!", credentials.ip, credentials.port)
        return MQTT_ESP_WRONG_ANSWER;
    } else if (espErrorCode == ESP_UART_IS_BUSY) {
        PRINT("Could not connect to %s at Port %s. UART busy!", credentials.ip, credentials.port)
        return MQTT_ESP_CHIP_FAILED;
    } else {
        PRINT("Could not connect to %s at Port %s", credentials.ip, credentials.port)
        return MQTT_CONNECTION_FAILED;
    }
}

void mqttBrokerDisconnect(bool force) {
    // check if force disconnect is requested!
    if (!force) {
        if (espStatus.ChipStatus == CONNECTED && espStatus.WIFIStatus == CONNECTED) {
            if (espStatus.MQTTStatus == NOT_CONNECTED) {
                PRINT("No connection to close!")
                return;
            }
        }
    }

    // generate disconnect string
    char *disconnect = malloc(AT_MQTT_DISCONNECT_FROM_BROKER_LENGTH);
    strcpy(disconnect, AT_MQTT_DISCONNECT_FROM_BROKER);

    // send disconnect command
    espErrorCode_t espErrorCode =
        espSendCommand(disconnect, AT_MQTT_DISCONNECT_FROM_BROKER_RESPONSE, 5000);
    free(disconnect);

    if (espErrorCode == ESP_NO_ERROR) {
        espStatus.MQTTStatus = NOT_CONNECTED;

        free(mqttBrokerBrokerId);
        mqttBrokerBrokerId = NULL;
        free(mqttBrokerClientId);
        mqttBrokerClientId = NULL;

        PRINT_DEBUG("MQTT Broker disconnected!")
    } else {
        PRINT("Could not disconnect MQTT broker.")
    }
}

void mqttBrokerReceive(char *response) {
    posting_t posting = {};
    if (mqttBrokerInternalHandleResponse(&posting, response)) {
        for (int i = 0; i < mqttBrokerNumberOfSubscriptions; ++i) {
            if (topicMatcherCheckIfTopicMatches(mqttBrokerSubscriptions[i].topic, posting.topic)) {
                mqttBrokerSubscriptions[i].subscriber.deliver(posting);
                break;
            }
        }
        free(posting.topic);
        free(posting.data);
    }
}

/* endregion */

/* region communicationEndpoint.h */

void communicationEndpointPublish(posting_t posting) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!")
        return;
    }
    posting.topic = mqttBrokerInternalConcatDomainAndClientWithTopic(posting.topic);
    communicationEndpointPublishRaw(posting);
    free(posting.topic);
}

void communicationEndpointPublishRemote(posting_t posting) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!")
        return;
    }
    posting.topic = mqttBrokerInternalConcatDomainWithTopic(posting.topic);
    communicationEndpointPublishRaw(posting);
    free(posting.topic);
}

void communicationEndpointPublishRaw(posting_t posting) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!")
        return;
    }

    size_t commandLength = AT_MQTT_PUBLISH_LENGTH + strlen(posting.topic) + strlen(posting.data);
    char *publishData = malloc(commandLength);
    if (posting.retain) {
        snprintf(publishData, commandLength, AT_MQTT_PUBLISH, posting.topic, posting.data, "1");
    } else {
        snprintf(publishData, commandLength, AT_MQTT_PUBLISH, posting.topic, posting.data, "0");
    }

    if (ESP_NO_ERROR != espSendCommand(publishData, AT_MQTT_PUBLISH_RESPONSE, 5000)) {
        PRINT("Could not publish to topic: %s.", posting.topic)
    } else {
        PRINT("Published to %s.", posting.topic)
    }

    free(publishData);
}

void communicationEndpointSubscribe(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic)
        return;
    }

    communicationEndpointSubscribeRaw(mqttBrokerInternalConcatDomainAndClientWithTopic(topic),
                                      subscriber);
}

void communicationEndpointSubscribeRemote(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic)
        return;
    }

    communicationEndpointSubscribeRaw(mqttBrokerInternalConcatDomainWithTopic(topic), subscriber);
}

void communicationEndpointSubscribeRaw(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic)
        return;
    }

    size_t commandLength = AT_MQTT_SUBSCRIBE_TOPIC_LENGTH + strlen(topic);
    char *subscribeTopic = malloc(commandLength);
    snprintf(subscribeTopic, commandLength, AT_MQTT_SUBSCRIBE_TOPIC, topic);

    if (mqttBrokerNumberOfSubscriptions != MAX_SUBSCRIBER) {
        if (ESP_NO_ERROR !=
            espSendCommand(subscribeTopic, AT_MQTT_SUBSCRIBE_TOPIC_RESPONSE, 5000)) {
            PRINT("Could not subscribe to topic: %s. Have You already subscribed?", topic)
        } else {
            mqttBrokerSubscriptions[mqttBrokerNumberOfSubscriptions] =
                (mqttBrokerSubscription_t){.topic = topic, .subscriber = subscriber};
            mqttBrokerNumberOfSubscriptions++;
            PRINT("Subscribed to %s", topic)
        }
    } else {
        PRINT("Could not subscribe to topic: %s. Maximum number of subscriptions reached.", topic)
    }

    free(subscribeTopic);
}

void communicationEndpointUnsubscribe(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic)
        return;
    }

    char *fullTopic = mqttBrokerInternalConcatDomainAndClientWithTopic(topic);
    communicationEndpointUnsubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void communicationEndpointUnsubscribeRemote(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic)
        return;
    }

    char *fullTopic = mqttBrokerInternalConcatDomainWithTopic(topic);
    communicationEndpointUnsubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void communicationEndpointUnsubscribeRaw(char *topic, subscriber_t subscriber) {
    if (espStatus.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic)
        return;
    }

    size_t commandLength = AT_MQTT_UNSUBSCRIBE_TOPIC_LENGTH + strlen(topic);
    char *command = malloc(commandLength);
    snprintf(command, commandLength, AT_MQTT_UNSUBSCRIBE_TOPIC, topic);

    if (ESP_NO_ERROR != espSendCommand(command, AT_MQTT_UNSUBSCRIBE_TOPIC_RESPONSE, 5000)) {
        PRINT("Could not unsubscribe from topic %s. Have you subscribed beforehand?", topic)
    } else {
        for (int i = 0; i < mqttBrokerNumberOfSubscriptions; ++i) {
            if (strcmp(mqttBrokerSubscriptions[i].topic, topic) == 0) {
                if (mqttBrokerSubscriptions[i].subscriber.deliver == subscriber.deliver) {
                    if (i != mqttBrokerNumberOfSubscriptions) {
                        strcpy(mqttBrokerSubscriptions[i].topic,
                               mqttBrokerSubscriptions[mqttBrokerNumberOfSubscriptions].topic);
                        mqttBrokerSubscriptions[i].subscriber =
                            mqttBrokerSubscriptions[mqttBrokerNumberOfSubscriptions].subscriber;
                    }
                    strcpy(mqttBrokerSubscriptions[mqttBrokerNumberOfSubscriptions].topic, "\0");
                    free(mqttBrokerSubscriptions[mqttBrokerNumberOfSubscriptions].topic);
                    mqttBrokerNumberOfSubscriptions--;
                }
            }
        }
        PRINT("Unsubscribed from %s.", topic)
    }

    free(command);
}

char *communicationEndpointGetBrokerDomain() {
    return mqttBrokerBrokerId;
}

char *communicationEndpointGetClientId() {
    return mqttBrokerClientId;
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static void mqttBrokerInternalSetBrokerDomain(char *ID) {
    // delete previous domain if needed
    if (mqttBrokerBrokerId != NULL) {
        free(mqttBrokerBrokerId);
    }

    // store new broker domain
    size_t brokerIdLength = strlen(ID);
    mqttBrokerBrokerId = malloc(brokerIdLength);
    memset(mqttBrokerBrokerId, '\0', brokerIdLength);
    strcpy(mqttBrokerBrokerId, ID);
}

static mqttBrokerErrorCode_t mqttBrokerInternalSetUserConfiguration(char *clientId, char *userId,
                                                                    char *password) {
    // delete previous ID if needed
    if (mqttBrokerClientId != NULL) {
        free(mqttBrokerClientId);
        mqttBrokerClientId = NULL;
    }

    // store new client id
    size_t clientIdLength = strlen(clientId);
    mqttBrokerClientId = malloc(clientIdLength);
    strcpy(mqttBrokerClientId, clientId);

    // generate command to send user configuration to esp module
    size_t commandLength =
        AT_MQTT_USER_CONFIGURATION_LENGTH + strlen(clientId) + strlen(userId) + strlen(password);
    char *setClientID = malloc(commandLength);
    snprintf(setClientID, commandLength, AT_MQTT_USER_CONFIGURATION, clientId, userId, password);

    // send user configuration to esp module
    espErrorCode_t espErrorCode =
        espSendCommand(setClientID, AT_MQTT_USER_CONFIGURATION_RESPONSE, 1000);
    free(setClientID);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT("Set client id to %s", clientId)
        return MQTT_NO_ERROR;
    } else if (espErrorCode == ESP_WRONG_ANSWER_RECEIVED) {
        PRINT("Could not set client id to %s, aborting...", clientId)
        return MQTT_ESP_WRONG_ANSWER;
    } else {
        PRINT("Could not set client id to %s, aborting...", clientId)
        return MQTT_ESP_CHIP_FAILED;
    }
}

static mqttBrokerErrorCode_t mqttBrokerInternalSetConnectionConfiguration(void) {
    // generate LWT topic
    char *lwt_topic = mqttBrokerInternalConcatDomainAndClientWithTopic("status");
    size_t lwt_topic_length = strlen(lwt_topic);

    // generate LWT message
    size_t lwt_message_length = strlen(mqttBrokerClientId) + 3;
    char *lwt_message = malloc(lwt_message_length);
    snprintf(lwt_message, lwt_message_length, "%s;0", mqttBrokerClientId);

    // generate command to send connection configuration to esp module
    size_t commandLength =
        AT_MQTT_CONNECTION_CONFIGURATION_LENGTH + lwt_topic_length + lwt_message_length;
    char *setConnectionConfiguration = malloc(commandLength);
    snprintf(setConnectionConfiguration, commandLength, AT_MQTT_CONNECTION_CONFIGURATION, lwt_topic,
             lwt_message);

    // send connection configuration to esp module
    espErrorCode_t espErrorCode =
        espSendCommand(setConnectionConfiguration, AT_MQTT_CONNECTION_CONFIGURATION_RESPONSE, 5000);
    free(setConnectionConfiguration);

    if (espErrorCode == ESP_NO_ERROR) {
        PRINT("Set connection configuration successful.")
        return MQTT_NO_ERROR;
    } else if (espErrorCode == ESP_WRONG_ANSWER_RECEIVED) {
        PRINT("Failed to store Connection settings! Wrong answer!")
        return MQTT_ESP_WRONG_ANSWER;
    } else if (espErrorCode == ESP_UART_IS_BUSY) {
        PRINT("Failed to store Connection settings! UART busy!")
        return MQTT_ESP_CHIP_FAILED;
    } else {
        PRINT("Failed to store Connection settings!")
        return MQTT_CONNECTION_FAILED;
    }
}

static void publishAliveStatusMessage() {
    // create alive message
    size_t messageLength = strlen(mqttBrokerClientId) + 3;
    char *message = malloc(messageLength);
    snprintf(message, messageLength, "%s;1", mqttBrokerClientId);
    posting_t aliveMessage = {.topic = "status", .data = message, .retain = 1};

    // publish message
    communicationEndpointPublish(aliveMessage);
}

static char *mqttBrokerInternalConcatDomainAndClientWithTopic(const char *topic) {
    size_t lengthOfResult =
        3 + strlen(mqttBrokerBrokerId) + strlen(mqttBrokerClientId) + strlen(topic);
    char *result = malloc(lengthOfResult);
    snprintf(result, lengthOfResult, "%s/%s/%s", mqttBrokerBrokerId, mqttBrokerClientId, topic);
    return result;
}

static char *mqttBrokerInternalConcatDomainWithTopic(const char *topic) {
    size_t lengthOfResult = 2 + strlen(mqttBrokerBrokerId) + strlen(topic);
    char *result = malloc(lengthOfResult);
    snprintf(result, lengthOfResult, "%s/%s", mqttBrokerBrokerId, topic);
    return result;
}

static void mqttBrokerInternalGetTopic(posting_t *posting, const char *startOfTopic,
                                       int lengthOfTopic) {
    char *topicBuffer = malloc(sizeof(char) * (lengthOfTopic + 1));
    memset(topicBuffer, '\0', lengthOfTopic + 1);
    strncpy(topicBuffer, startOfTopic, lengthOfTopic);
    posting->topic = topicBuffer;
}

static int mqttBrokerInternalGetNumberOfDataBytes(const char *startOfNumber,
                                                  const char *endOfNumber) {
    int lengthOfNumber = endOfNumber - startOfNumber;
    char *numberString = malloc(sizeof(char) * (lengthOfNumber + 1));
    memset(numberString, '\0', lengthOfNumber + 1);
    strncpy(numberString, startOfNumber, lengthOfNumber);
    int dataLength = strtol(numberString, NULL, 10);
    free(numberString);
    return dataLength;
}

static void mqttBrokerInternalGetData(posting_t *posting, const char *startOfData, int dataLength) {
    char *dataBuffer = malloc(sizeof(char) * (dataLength + 1));
    memset(dataBuffer, '\0', dataLength + 1);
    strncpy(dataBuffer, startOfData, dataLength);
    posting->data = dataBuffer;
}

static bool mqttBrokerInternalHandleResponse(posting_t *posting, char *response) {
    if (strlen(response) == 0) {
        PRINT_DEBUG("Empty Response.")
        return false;
    }

    char *startOfTopic = strstr(response, ",\"") + 2;
    char *endOfTopic = strstr(startOfTopic, "\",");
    mqttBrokerInternalGetTopic(posting, startOfTopic, endOfTopic - startOfTopic);
    PRINT_DEBUG("Got topic: %s", posting->topic)

    char *startOfDataLength = endOfTopic + 2;
    char *endOfDataLength = strstr(startOfDataLength, ",");
    int dataLength = mqttBrokerInternalGetNumberOfDataBytes(startOfDataLength, endOfDataLength);
    PRINT_DEBUG("Got length of Data: %i", dataLength)

    mqttBrokerInternalGetData(posting, endOfDataLength + 1, dataLength);
    PRINT_DEBUG("Got data: %s", posting->data)

    return true;
}

/* endregion */
