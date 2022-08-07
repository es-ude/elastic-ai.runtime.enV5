#define SOURCE_FILE "MQTT-BROKER"

#include <stdlib.h>
#include <string.h>

#include "MQTTBroker.h"
#include "MQTTBroker_internal.h"
#include "Network.h"
#include "at_commands.h"
#include "common.h"
#include "esp.h"

// header from elastic-ai.runtime.c
#include "communicationEndpoint.h"
#include "posting.h"
#include "subscriber.h"
#include "topicMatcher.h"

/* region VARIABLES */

char *MQTT_Broker_brokerID = NULL;
char *MQTT_Broker_clientID = NULL;
uint8_t MQTT_NumberOfSubscriptions = 0;
Subscription MQTT_Subscriptions[MAX_SUBSCRIBER];
bool MQTT_BROKER_ReceiverFunctionSet = false;

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

void mqtt_ConnectToBrokerUntilSuccessful(MQTTHost_t mqttHost, char *brokerDomain, char *clientID) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Could not connect to MQTT broker! Chip problem.")
        return;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT("Could not connect to MQTT broker! No Wifi connection.")
        return;
    }

    while (!mqtt_ConnectToBroker(mqttHost, brokerDomain, clientID)) {
        PRINT_DEBUG("Connection failed. Trying again now!");
    }
}

bool mqtt_ConnectToBroker(MQTTHost_t credentials, char *brokerDomain, char *clientID) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Could not connect to MQTT broker! Chip problem.")
        return false;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT("Could not connect to MQTT broker! No Wifi connection.")
        return false;
    }
    if (ESP_Status.MQTTStatus == CONNECTED) {
        PRINT("MQTT Broker already connected! Disconnect first")
        return true;
    }

    mqtt_SetClientId(clientID);
    mqtt_setBrokerDomain(brokerDomain);

    size_t commandLength =
        AT_MQTT_CONNECT_TO_BROKER_LENGTH + strlen(credentials.ip) + strlen(credentials.port);
    char *connectToBroker = malloc(commandLength);
    snprintf(connectToBroker, commandLength, AT_MQTT_CONNECT_TO_BROKER, credentials.ip,
             credentials.port);

    if (esp_SendCommand(connectToBroker, AT_MQTT_CONNECT_TO_BROKER_RESPONSE, 5000)) {
        ESP_Status.MQTTStatus = CONNECTED;
        if (!MQTT_BROKER_ReceiverFunctionSet) {
           esp_SetMQTTReceiverFunction(mqtt_Receive);
            MQTT_BROKER_ReceiverFunctionSet = true;
        }
        PRINT("Connected to %s at Port %s", credentials.ip, credentials.port)
        free(connectToBroker);
        return true;
    } else {
        PRINT("Could not connect to %s at Port %s", credentials.ip, credentials.port)
        free(connectToBroker);
        return false;
    }
}

void mqtt_Disconnect(bool force) {
    if (!force) {
        if (ESP_Status.ChipStatus == CONNECTED && ESP_Status.WIFIStatus == CONNECTED) {
            if (ESP_Status.MQTTStatus == NOT_CONNECTED) {
                PRINT("No connection to close!")
                return;
            }
        }
    }

    if (esp_SendCommand(AT_MQTT_DISCONNECT_FROM_BROKER, AT_MQTT_DISCONNECT_FROM_BROKER_RESPONSE,
                        5000)) {
        ESP_Status.MQTTStatus = NOT_CONNECTED;

        free(MQTT_Broker_brokerID);
        MQTT_Broker_brokerID = NULL;
        free(MQTT_Broker_clientID);
        MQTT_Broker_clientID = NULL;

        PRINT_DEBUG("MQTT Broker disconnected!")
    } else {
        PRINT("Could not disconnect MQTT broker.")
    }
}

void mqtt_setBrokerDomain(char *ID) {
    if (MQTT_Broker_brokerID != NULL) {
        free(MQTT_Broker_brokerID);
    }
    size_t brokerIdLength = strlen(ID);
    MQTT_Broker_brokerID = malloc(brokerIdLength);
    memset(MQTT_Broker_brokerID, '\0', brokerIdLength);
    strcpy(MQTT_Broker_brokerID, ID);
}

void mqtt_SetClientId(char *clientId) {
    if (MQTT_Broker_clientID != NULL) {
        free(MQTT_Broker_clientID);
    }
    size_t clientIdLength = strlen(clientId);
    MQTT_Broker_clientID = malloc(clientIdLength);
    memset(MQTT_Broker_clientID, '\0', clientIdLength);
    strcpy(MQTT_Broker_clientID, clientId);

    size_t commandLength = AT_MQTT_USER_CONFIGURATION_LENGTH + strlen(clientId);
    char *setClientID = malloc(commandLength);
    snprintf(setClientID, commandLength, AT_MQTT_USER_CONFIGURATION, clientId);

    if (!esp_SendCommand(setClientID, AT_MQTT_USER_CONFIGURATION_RESPONSE, 1000)) {
        PRINT("Could not set client id to %s, aborting...", clientId)
    }

    free(setClientID);
}

void mqtt_Receive(char *response) {
    Posting posting = {};
    if (handleResponse(&posting, response)) {
        for (int i = 0; i < MQTT_NumberOfSubscriptions; ++i) {
            if (checkIfTopicMatches(MQTT_Subscriptions[i].topic, posting.topic)) {
                MQTT_Subscriptions[i].subscriber.deliver(posting);
                break;
            }
        }
        free(posting.topic);
        free(posting.data);
    }
}

/* endregion */

/* region communicationEndpoint.h */

void publish(Posting posting) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't publish data!")
        return;
    }
    
    char *fullAddress = concatDomainAndClientWithTopic(posting.topic);
    size_t commandLength =
             sizeof(char) * (AT_MQTT_PUBLISH_LENGTH + strlen(fullAddress) + strlen(posting.data));
    char *command = malloc(commandLength);
    snprintf(command, commandLength, AT_MQTT_PUBLISH, fullAddress, posting.data);
    
    if (!esp_SendCommand(command, AT_MQTT_PUBLISH_RESPONSE, 5000)) {
        PRINT("Could not publish to fullAddress: %s.", fullAddress)
    } else {
        PRINT("Published to %s.", fullAddress)
    }
    
    free(command);
    free(fullAddress);
}

void publishRaw(char *topic, Posting posting) {
    // TODO: Implement function publishRaw
}

void subscribe(char *topic, Subscriber subscriber) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't subscribe to topic %s!", topic)
        return;
    }
    subscribeRaw(concatDomainWithTopic(topic), subscriber);
}

void subscribeRaw(char *topic, Subscriber subscriber) {
    size_t commandLength = AT_MQTT_SUBSCRIBE_TOPIC_LENGTH + strlen(topic);
    char *command = malloc(commandLength);
    snprintf(command, commandLength, AT_MQTT_SUBSCRIBE_TOPIC, topic);

    if (MQTT_NumberOfSubscriptions != MAX_SUBSCRIBER) {
        if (!esp_SendCommand(command, AT_MQTT_SUBSCRIBE_TOPIC_RESPONSE, 5000)) {
            PRINT("Could not subscribe to topic: %s. Have You already subscribed?", topic)
        } else {
            MQTT_Subscriptions[MQTT_NumberOfSubscriptions] =
                (Subscription){.topic = topic, .subscriber = subscriber};
            MQTT_NumberOfSubscriptions++;
            PRINT("Subscribed to %s", topic)
        }
    } else {
        PRINT("Could not subscribe to topic: %s. Maximum number of subscriptions reached.", topic)
    }

    free(command);
}

void unsubscribe(char *topic, Subscriber subscriber) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED) {
        PRINT("MQTT broker not connected. Can't unsubscribe from topic %s!", topic)
        return;
    }

    char *fullAddress = concatDomainWithTopic(topic);
    unsubscribeRaw(fullAddress, subscriber);
    free(fullAddress);
}

void unsubscribeRaw(char *topic, Subscriber subscriber) {
    size_t commandLength = AT_MQTT_UNSUBSCRIBE_TOPIC_LENGTH + strlen(topic);
    char *command = malloc(commandLength);
    snprintf(command, commandLength, AT_MQTT_UNSUBSCRIBE_TOPIC, topic);

    if (!esp_SendCommand(command, AT_MQTT_UNSUBSCRIBE_TOPIC_RESPONSE, 5000)) {
        PRINT("Could not unsubscribe from topic %s. Have you subscribed beforehand?", topic)
    } else {
        for (int i = 0; i < MQTT_NumberOfSubscriptions; ++i) {
            if (strcmp(MQTT_Subscriptions[i].topic, topic) == 0) {
                if (MQTT_Subscriptions[i].subscriber.deliver == subscriber.deliver) {
                    if (i != MQTT_NumberOfSubscriptions) {
                        strcpy(MQTT_Subscriptions[i].topic,
                               MQTT_Subscriptions[MQTT_NumberOfSubscriptions].topic);
                        MQTT_Subscriptions[i].subscriber =
                            MQTT_Subscriptions[MQTT_NumberOfSubscriptions].subscriber;
                    }
                    strcpy(MQTT_Subscriptions[MQTT_NumberOfSubscriptions].topic, "\0");
                    free(MQTT_Subscriptions[MQTT_NumberOfSubscriptions].topic);
                    MQTT_NumberOfSubscriptions--;
                }
            }
        }
        PRINT("Unsubscribed from %s.", topic)
    }

    free(command);
}

char *getID() {
    return MQTT_Broker_brokerID;
}

void setID(char *ID) {
    // TODO: Implement function setID
}

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static char *concatDomainAndClientWithTopic(const char *topic) {
    size_t lengthOfResult =
               3 + strlen(MQTT_Broker_brokerID) + strlen(MQTT_Broker_clientID) + strlen(topic);
    char *result = malloc(lengthOfResult);
    snprintf(result, lengthOfResult, "%s/%s/%s", MQTT_Broker_brokerID, MQTT_Broker_clientID, topic);
    return result;
}

static char *concatDomainWithTopic(const char *topic) {
    size_t lengthOfResult = 2 + strlen(MQTT_Broker_brokerID) + strlen(topic);
    char *result = malloc(lengthOfResult);
    snprintf(result, lengthOfResult, "%s/%s", MQTT_Broker_brokerID, topic);
    return result;
}

static void getTopic(Posting *posting, const char *startOfTopic, int lengthOfTopic) {
    char *topicBuffer = malloc(sizeof(char) * (lengthOfTopic + 1));
    memset(topicBuffer, '\0', lengthOfTopic + 1);
    strncpy(topicBuffer, startOfTopic, lengthOfTopic);
    posting->topic = topicBuffer;
}

static int getNumberOfDataBytes(const char *startOfNumber, const char *endOfNumber) {
    int lengthOfNumber = endOfNumber - startOfNumber;
    char *numberString = malloc(sizeof(char) * (lengthOfNumber + 1));
    memset(numberString, '\0', lengthOfNumber + 1);
    strncpy(numberString, startOfNumber, lengthOfNumber);
    int dataLength = strtol(numberString, NULL, 10);
    free(numberString);
    return dataLength;
}

static void getData(Posting *posting, const char *startOfData, int dataLength) {
    char *dataBuffer = malloc(sizeof(char) * (dataLength + 1));
    memset(dataBuffer, '\0', dataLength + 1);
    strncpy(dataBuffer, startOfData + 1, dataLength);
    posting->data = dataBuffer;
}

static bool handleResponse(Posting *posting, char *response) {
    if (strlen(response) == 0) {
        PRINT_DEBUG("Empty Response.")
        return false;
    }

    char *startOfTopic = strstr(response, ",\"") + 2;
    char *endOfTopic = strstr(startOfTopic, "\",");
    getTopic(posting, startOfTopic, endOfTopic - startOfTopic);
    PRINT_DEBUG("Got topic %s from response", posting->topic)

    char *startOfData = endOfTopic + 2;
    char *endOfData = strstr(startOfData, ",");
    int dataLength = getNumberOfDataBytes(startOfData, endOfData);
    PRINT_DEBUG("Got length of Data: %i", dataLength)

    getData(posting, endOfTopic, dataLength);
    PRINT_DEBUG("Got data: %s", posting->data)

    return true;
}

/* endregion */
