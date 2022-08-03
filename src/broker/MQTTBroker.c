#define SOURCE_FILE "MQTT-BROKER"

#include "MQTTBroker.h"
#include "MQTTBroker_internal.h"
#include "Network.h"
#include "common.h"
#include "communicationEndpoint.h"
#include "esp.h"
#include "posting.h"
#include "subscriber.h"
#include "topicMatcher.h"
#include <stdlib.h>
#include <string.h>

char *MQTT_Broker_brokerID = NULL;
char *MQTT_Broker_clientID = NULL;
uint8_t MQTT_Broker_numberSubscriber = 0;
Subscription MQTT_Broker_subscriberList[MAX_SUBSCRIBER];
bool MQTT_BROKER_ReceiverFunctionSet = false;

void MQTT_Broker_ConnectToBrokerUntilConnected(MQTTHost_t credentials, char *brokerDomain,
                                               char *clientID) {
    if (ESP_Status.MQTTStatus == CONNECTED)
        return;
    while (!MQTT_Broker_ConnectToBroker(credentials, brokerDomain, clientID))
        ;
}

bool MQTT_Broker_ConnectToBroker(MQTTHost_t credentials, char *brokerDomain, char *clientID) {
    if (ESP_Status.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Could not connect to MQTT broker. Chip problem.")
        return true;
    }
    if (ESP_Status.WIFIStatus == NOT_CONNECTED) {
        PRINT("Could not connect to MQTT broker. No Wifi connection.")
        return true;
    }
    if (ESP_Status.MQTTStatus == CONNECTED) {
        PRINT("There is already a MQTT Connection open. Please close this one "
              "first")
        return true;
    }

    MQTT_Broker_SetClientId(clientID);
    MQTT_Broker_setBrokerDomain(brokerDomain);

    size_t commandLength = strlen(credentials.ip) + strlen(credentials.port) + 20;
    char *command = malloc(commandLength);
    snprintf(command, commandLength, "AT+MQTTCONN=0,\"%s\",%s,0", credentials.ip, credentials.port);

    if (esp_SendCommand(command, "+MQTTCONNECTED", 5000)) {
        ESP_Status.MQTTStatus = CONNECTED;
        if (!MQTT_BROKER_ReceiverFunctionSet) {
            esp_SetMQTTReceiverFunction(MQTT_Broker_Receive);
            MQTT_BROKER_ReceiverFunctionSet = true;
        }
        PRINT("Connected to %s at Port %s", credentials.ip, credentials.port)
    } else {
        PRINT("Could not connect to %s at Port %s", credentials.ip, credentials.port)
        free(command);
        return false;
    }

    free(command);
    return true;
}

void MQTT_Broker_Disconnect(bool force) {
    if (!force) {
        if (ESP_Status.ChipStatus && ESP_Status.WIFIStatus == CONNECTED) {
            if (ESP_Status.MQTTStatus == NOT_CONNECTED) {
                PRINT("No connection to close")
                return;
            }
        }
    }

    if (esp_SendCommand("AT+MQTTCLEAN=0", "OK", 5000)) {
        ESP_Status.MQTTStatus = NOT_CONNECTED;

        free(MQTT_Broker_brokerID);
        MQTT_Broker_brokerID = NULL;
        free(MQTT_Broker_clientID);
        MQTT_Broker_clientID = NULL;
    } else {
        PRINT("Could not disconnect MQTT connection.")
    }
}

void MQTT_Broker_setBrokerDomain(char *ID) {
    if (MQTT_Broker_brokerID != NULL) {
        free(MQTT_Broker_brokerID);
    }
    size_t brokerIdLength = strlen(ID);
    MQTT_Broker_brokerID = malloc(brokerIdLength);
    snprintf(MQTT_Broker_brokerID, brokerIdLength, ID);
}

void MQTT_Broker_SetClientId(char *clientId) {
    if (MQTT_Broker_clientID != NULL) {
        free(MQTT_Broker_clientID);
    }
    size_t clientIdLength = strlen(clientId);
    MQTT_Broker_clientID = malloc(clientIdLength);
    snprintf(MQTT_Broker_clientID, clientIdLength, clientId);

    size_t commandLength = strlen(clientId) + 35;
    char *command = malloc(commandLength);
    snprintf(command, commandLength, "AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"", clientId);

    if (!esp_SendCommand(command, "OK", 1000)) {
        PRINT("Could not set client id to %s, aborting...", clientId)
    }

    free(command);
}

void MQTT_Broker_Receive(char *response) {
    Posting posting = {};
    if (MQTT_Broker_HandleResponse(&posting, response)) {
        for (int i = 0; i < MQTT_Broker_numberSubscriber; ++i) {
            if (checkIfTopicMatches(MQTT_Broker_subscriberList[i].topic, posting.topic)) {
                MQTT_Broker_subscriberList[i].subscriber.deliver(posting);
            }
        }
        free(posting.topic);
        free(posting.data);
    }
}

void MQTT_Broker_getTopic(Posting *posting, const char *start, int lengthOfTopic) {
    char *topicBuffer = malloc(sizeof(char) * (lengthOfTopic + 1));
    memset(topicBuffer, '\0', lengthOfTopic + 1);
    strncpy(topicBuffer, start, lengthOfTopic);
    posting->topic = topicBuffer;
}

int MQTT_Broker_getDataLength(char *start, const char *end) {
    int lengthOfLength = end - start;
    char *lengthBuffer = malloc(sizeof(char) * (lengthOfLength + 1));
    memset(lengthBuffer, '\0', lengthOfLength + 1);
    strncpy(lengthBuffer, start, lengthOfLength);
    int dataLength = strtol(lengthBuffer, NULL, 10);
    free(lengthBuffer);
    return dataLength;
}

void MQTT_Broker_getData(Posting *posting, const char *end, int dataLength) {
    char *dataBuffer = malloc(sizeof(char) * (dataLength + 1));
    memset(dataBuffer, '\0', dataLength + 1);
    strncpy(dataBuffer, end + 1, dataLength);
    posting->data = dataBuffer;
}

bool MQTT_Broker_HandleResponse(Posting *posting, char *response) {
    if (strlen(response) == 0) {
        return false;
    }

    char *start = strstr(response, ",\"") + 2;
    char *end = strstr(start, "\",");

    int lengthOfTopic = end - start;
    MQTT_Broker_getTopic(posting, start, lengthOfTopic);

    start = end + 2;
    end = strstr(start, ",");
    int dataLength = MQTT_Broker_getDataLength(start, end);

    MQTT_Broker_getData(posting, end, dataLength);

    return true;
}

char *MQTT_Broker_concatDomainAndClientWithTopic(const char *topic) {
    size_t lengthOfResult =
        3 + strlen(MQTT_Broker_brokerID) + strlen(MQTT_Broker_clientID) + strlen(topic);
    char *result = malloc(lengthOfResult);
    snprintf(result, lengthOfResult, "%s/%s/%s", MQTT_Broker_brokerID, MQTT_Broker_clientID, topic);
    return result;
}

char *MQTT_Broker_concatDomainWithTopic(const char *topic) {
    size_t lengthOfResult = 2 + strlen(MQTT_Broker_brokerID) + strlen(topic);
    char *result = malloc(lengthOfResult);
    snprintf(result, lengthOfResult, "%s/%s", MQTT_Broker_brokerID, topic);
    return result;
}

void publish(Posting posting) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED)
        return;

    char *topic = MQTT_Broker_concatDomainAndClientWithTopic(posting.topic);
    size_t commandLength = sizeof(char) * (strlen(topic) + strlen(posting.data) + 23);
    char *command = malloc(commandLength);
    // Quality of service 0 - 2 see MQTT documentation
    snprintf(command, commandLength, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0", topic, posting.data);

    if (!esp_SendCommand(command, "OK", 1000)) {
        PRINT("Could not publish to topic: %s.", topic)
    } else {
        PRINT("Published to %s.", topic)
    }

    free(command);
    free(topic);
}

void subscribe(char *topic, Subscriber subscriber) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED)
        return;
    subscribeRaw(MQTT_Broker_concatDomainWithTopic(topic), subscriber);
}

void subscribeRaw(char *topic, Subscriber subscriber) {
    size_t commandLength = 18 + strlen(topic);
    char *command = malloc(commandLength);
    // Quality of service 0 - 2 see MQTT documentation
    snprintf(command, commandLength, "AT+MQTTSUB=0\"%s\",0", topic);

    if (MQTT_Broker_numberSubscriber != MAX_SUBSCRIBER) {
        if (!esp_SendCommand(command, "OK", 1000)) {
            PRINT("Could not subscribe to topic: %s. Have You already "
                  "subscribed?",
                  topic)
        } else {
            MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber] =
                (Subscription){.topic = topic, .subscriber = subscriber};
            MQTT_Broker_numberSubscriber++;
            PRINT("Subscribed to %s", topic)
        }
    } else {
        PRINT("Could not subscribe to topic: %s. Maximum number of "
              "subscriptions reached.",
              topic)
    }

    free(command);
}

void unsubscribe(char *topic, Subscriber subscriber) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED)
        return;
    char *fullTopic = MQTT_Broker_concatDomainWithTopic(topic);
    unsubscribeRaw(fullTopic, subscriber);
    free(fullTopic);
}

void unsubscribeRaw(char *topic, Subscriber subscriber) {
    size_t commandLength = 18 + strlen(topic);
    char *command = malloc(commandLength);
    snprintf(command, commandLength, "AT+MQTTUNSUB=0,\"%s\"", topic);

    if (!esp_SendCommand(command, "OK", 1000)) {
        PRINT("Could not unsubscribe to topic: %s. Have you subscribed "
              "beforehand?",
              topic)
    } else {
        for (int i = 0; i < MQTT_Broker_numberSubscriber; ++i) {
            if (strcmp(MQTT_Broker_subscriberList[i].topic, topic) == 0) {
                if (MQTT_Broker_subscriberList[i].subscriber.deliver == subscriber.deliver) {
                    if (i != MQTT_Broker_numberSubscriber) {
                        strcpy(MQTT_Broker_subscriberList[i].topic,
                               MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].topic);
                        MQTT_Broker_subscriberList[i].subscriber =
                            MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].subscriber;
                    }
                    strcpy(MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].topic, "\0");
                    free(MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].topic);
                    MQTT_Broker_numberSubscriber--;
                }
            }
        }
        PRINT("Unsubscribed from %s.", topic)
    }

    free(command);
}

char *ID() {
    return MQTT_Broker_brokerID;
}
