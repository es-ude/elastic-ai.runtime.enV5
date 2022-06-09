#define SOURCE_FILE "MQTT-BROKER"

#include "MQTTBroker.h"
#include "MQTTBroker_internal.h"
#include "common.h"
#include "esp.h"
#include "Network.h"
#include "subscriber.h"
#include "posting.h"
#include "communicationEndpoint.h"
#include "topicMatcher.h"
#include "TaskWrapper.h"
#include <string.h>
#include <stdlib.h>

char *MQTT_Broker_brokerDomain = NULL;
char *MQTT_Broker_clientID = NULL;
uint8_t MQTT_Broker_numberSubscriber = 0;
Subscription MQTT_Broker_subscriberList[MAX_SUBSCRIBER];
bool MQTT_BROKER_ReceiverFunctionSet = false;

void MQTT_Broker_ConnectToBroker(char *target, char *port, char *brokerDomain, char *clientID) {
    if (NetworkStatus.ChipStatus == ESP_CHIP_NOT_OK) {
        PRINT("Could not connect to MQTT broker. Chip problem.")
        return;
    }
    if (NetworkStatus.WIFIStatus == NOT_CONNECTED) {
        PRINT("Could not connect to MQTT broker. No Wifi connection.")
        return;
    }
    if (NetworkStatus.MQTTStatus == CONNECTED) {
        PRINT("There is already a MQTT Connection open. Please close this one first")
        return;
    }

    MQTT_Broker_SetClientId(clientID);
    MQTT_Broker_setBrokerDomain(brokerDomain);

    char cmd[100];
    strcpy(cmd, "AT+MQTTCONN=0,\"");
    strcat(cmd, target);
    strcat(cmd, "\",");
    strcat(cmd, port);
    strcat(cmd, ",0");
    if (ESP_SendCommand(cmd, "+MQTTCONNECTED", 5000)) {
        PRINT("Connected to %s at Port %s", target, port)
        TaskSleep(2000);
        NetworkStatus.MQTTStatus = CONNECTED;
        if (!MQTT_BROKER_ReceiverFunctionSet) {
            ESP_SetMQTTReceiverFunction(MQTT_Broker_Receive);
            MQTT_BROKER_ReceiverFunctionSet = true;
        }
    } else {
        PRINT("Could not connect to %s at Port %s", target, port)
    }
}

void MQTT_Broker_Disconnect(bool force) {
    if (!force) {
        if (NetworkStatus.ChipStatus && NetworkStatus.WIFIStatus == CONNECTED) {
            if (NetworkStatus.MQTTStatus == NOT_CONNECTED) {
                PRINT("No connection to close")
                return;
            }
        }
    }

    ESP_SendCommandForce("AT+MQTTCLEAN=0", "OK", 5000);
    NetworkStatus.MQTTStatus = NOT_CONNECTED;

    free(MQTT_Broker_brokerDomain);
    free(MQTT_Broker_clientID);
}

void MQTT_Broker_setBrokerDomain(char *ID) {
    if (MQTT_Broker_brokerDomain != NULL) {
        free(MQTT_Broker_brokerDomain);
    }
    MQTT_Broker_brokerDomain = malloc(strlen(ID));
    strcpy(MQTT_Broker_brokerDomain, ID);
}

void MQTT_Broker_SetClientId(char *clientId) {
    if (MQTT_Broker_clientID != NULL) {
        free(MQTT_Broker_clientID);
    }
    MQTT_Broker_clientID = malloc(strlen(clientId));
    strcpy(MQTT_Broker_clientID, clientId);

    char cmd[100];
    strcpy(cmd, "AT+MQTTUSERCFG=0,1,\"");
    strcat(cmd, clientId);
    strcat(cmd, "\",\"\",\"\",0,0,\"\"");
    if (!ESP_SendCommand(cmd, "OK", 1000)) {
        PRINT("Could not set client id to %s, aborting...", clientId)
    }
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

bool MQTT_Broker_HandleResponse(Posting *posting, char *response) {
    if (strlen(response) == 0) {
        return false;
    }

    char *start;
    char *end;
    start = strstr(response, ",\"") + 2;
    end = strstr(start, "\",");
    int lengthOfTopic = end - start;
    char *topicBuffer = malloc(sizeof(char) * (lengthOfTopic + 1));
    memset(topicBuffer, '\0', lengthOfTopic + 1);
    strncpy(topicBuffer, start, lengthOfTopic);
    posting->topic = topicBuffer;

    start = end + 2;
    end = strstr(start, ",");
    int lengthOfLength = end - start;
    char *lengthBuffer = malloc(sizeof(char) * (lengthOfLength + 1));
    memset(lengthBuffer, '\0', lengthOfLength + 1);
    strncpy(lengthBuffer, start, lengthOfLength);
    int dataLength = strtol(lengthBuffer, NULL, 10);

    char *dataBuffer = malloc(sizeof(char) * (dataLength + 1));
    memset(dataBuffer, '\0', dataLength + 1);
    strncpy(dataBuffer, end + 1, dataLength);
    posting->data = dataBuffer;

    free(lengthBuffer);

    return true;
}

char *MQTT_Broker_concatIDWithTopic(const char *topic) {
    char *result = malloc(strlen(MQTT_Broker_brokerDomain) + strlen(MQTT_Broker_clientID) + strlen(topic) + 2);
    strcpy(result, MQTT_Broker_brokerDomain);
    strcat(result, "/");
    strcat(result, MQTT_Broker_clientID);
    strcat(result, "/");
    strcat(result, topic);
    return result;
}

void publish(Posting posting) {
    if (NetworkStatus.MQTTStatus == NOT_CONNECTED)
        return;

    char *topic = MQTT_Broker_concatIDWithTopic(posting.topic);

    char *cmd1 = "AT+MQTTPUB=0,\"";
    char *cmd2 = "\",\"";
    // Quality of service 0 - 2 see MQTT documentation
    char *cmd3 = "\",0,0";
    char *command = malloc(sizeof(char) * (
            strlen(cmd1) +
            strlen(cmd2) +
            strlen(cmd3) +
            strlen(topic) +
            strlen(posting.data) + 1));
    sprintf(command, "%s%s%s%s%s", cmd1, topic, cmd2, posting.data, cmd3);

    if (!ESP_SendCommand(command, "OK", 1000)) {
        PRINT("Could not publish to topic: %s.", topic)
    } else {
        PRINT("Published to %s.", topic)
    }
    free(command);
    free(topic);
}

void subscribe(char *topic, Subscriber subscriber) {
    if (NetworkStatus.MQTTStatus == NOT_CONNECTED)
        return;
    subscribeRaw(MQTT_Broker_concatIDWithTopic(topic), subscriber);
}

void subscribeRaw(char *topic, Subscriber subscriber) {
    char command[50];
    strcpy(command, "AT+MQTTSUB=0,\"");
    strcat(command, topic);
    // Quality of service 0 - 2 see MQTT documentation
    strcat(command, "\",0");

    if (MQTT_Broker_numberSubscriber != MAX_SUBSCRIBER) {
        if (!ESP_SendCommand(command, "OK", 1000)) {
            PRINT("Could not subscribe to topic: %s. Have You already subscribed?", topic)
        } else {
            MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber] = (Subscription) {.topic=topic, .subscriber=subscriber};
            MQTT_Broker_numberSubscriber++;
            PRINT("Subscribed to %s", topic)
        }
    } else {
        PRINT("Could not subscribe to topic: %s. Maximum number of subscriptions reached.", topic)
    }
}

void unsubscribe(char *topic, Subscriber subscriber) {
    if (NetworkStatus.MQTTStatus == NOT_CONNECTED)
        return;
    unsubscribeRaw(MQTT_Broker_concatIDWithTopic(topic), subscriber);
    free(topic);
}

void unsubscribeRaw(char *topic, Subscriber subscriber) {
    char command[50];
    strcpy(command, "AT+MQTTUNSUB=0,\"");
    strcat(command, topic);
    strcat(command, "\"");

    if (!ESP_SendCommand(command, "OK", 1000)) {
        PRINT("Could not unsubscribe to topic: %s. Have you subscribed beforehand?", topic)
    } else {
        for (int i = 0; i < MQTT_Broker_numberSubscriber; ++i) {
            if (strcmp(MQTT_Broker_subscriberList[i].topic, topic) == 0) {
                if (MQTT_Broker_subscriberList[i].subscriber.deliver == subscriber.deliver) {

                    strcpy(MQTT_Broker_subscriberList[i].topic,
                           MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].topic);
                    MQTT_Broker_subscriberList[i].subscriber = MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].subscriber;
                    strcpy(MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].topic, "\0");
                    free(MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber].topic);
                    MQTT_Broker_numberSubscriber--;
                }
            }
        }
        PRINT("Unsubscribed from %s.", topic)
    }
}

char *ID() {
    return MQTT_Broker_brokerDomain;
}
