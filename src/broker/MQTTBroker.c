#define SOURCE_FILE "MQTT-BROKER"

#include <string.h>
#include <stdlib.h>

#include "MQTTBroker.h"

#include "posting.h"
#include "protocol.h"
#include "communicationEndpoint.h"

#include "common.h"
#include "esp.h"
#include "uartToESP.h"
#include "TaskWrapper.h"
#include "Network.h"

char *MQTT_Broker_brokerDomain = "";
bool MQTT_Broker_ReceiverTaskRegistered = false;
uint MQTT_Broker_numberSubscriber = 0;
Subscription MQTT_Broker_subscriberList[MAX_SUBSCRIBER];

_Noreturn void MQTT_Broker_ReceiverTask();

bool MQTT_Broker_checkIfTopicMatches(char *subscribedTopic, char *publishedTopic);

void setID(char *ID) {
    MQTT_Broker_brokerDomain = ID;
}

void MQTT_Broker_ConnectToBroker(char *target, char *port) {
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

    char cmd[100];
    strcpy(cmd, "AT+MQTTCONN=0,\"");
    strcat(cmd, target);
    strcat(cmd, "\",");
    strcat(cmd, port);
    strcat(cmd, ",0");
    if (ESP_SendCommand(cmd, "+MQTTCONNECTED", 5000)) {
        PRINT("Connected to %s at Port %s", target, port)
        NetworkStatus.MQTTStatus = CONNECTED;
        if (!MQTT_Broker_ReceiverTaskRegistered)
            RegisterTask(MQTT_Broker_ReceiverTask, "_MQTT_BROKER_ReceiverTask");
    } else {
        PRINT("Could not connect to %s at Port %s", target, port)
    }
}

char *MQTT_Broker_concatIDWithTopic(const char *topic) {
    char *result = malloc(strlen(MQTT_Broker_brokerDomain) + strlen(topic) + 1);
    strcpy(result, MQTT_Broker_brokerDomain);
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

void unsubscribe(char *topic, Subscriber subscriber) {
    if (NetworkStatus.MQTTStatus == NOT_CONNECTED)
        return;
    unsubscribeRaw(MQTT_Broker_concatIDWithTopic(topic), subscriber);
}

void subscribeRaw(char *topic, Subscriber subscriber) {
    char command[50];
    strcpy(command, "AT+MQTTSUB=0,\"");
    strcat(command, topic);
    // Quality of service 0 - 2 see MQTT documentation
    strcat(command, "\",0");

    if (!ESP_SendCommand(command, "OK", 1000)) {
        PRINT("Could not subscribe to topic: %s. Have You already subscribed?", topic)
    } else {
        if (MQTT_Broker_numberSubscriber != MAX_SUBSCRIBER) {
            MQTT_Broker_subscriberList[MQTT_Broker_numberSubscriber] = (Subscription) {.topic=topic, .subscriber=subscriber};
            MQTT_Broker_numberSubscriber++;
            PRINT("Subscribed to %s", topic)
        } else {
            PRINT("Could not subscribe to topic: %s. Maximum number of subscriptions reached.", topic)
        }
    }
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

bool MQTT_Broker_IsResponseAvailable() {
    return uartToESP_ResponseArrived("+MQTTSUBRECV:0,");
}

static bool MQTT_Broker_CutResponseToBuffer(Posting *response);

bool MQTT_Broker_GetResponse(Posting *response) {
    ASSERT(MQTT_Broker_IsResponseAvailable())
    return MQTT_Broker_CutResponseToBuffer(response);
}

bool MQTT_Broker_CutResponseToBuffer(Posting *response) {
#define DIGITS_OF_LENGTH 4
    char metaDataBuf[MAX_TOPIC_NAME_LENGTH + DIGITS_OF_LENGTH + 1];
    char *cmd = "+MQTTSUBRECV:0,\"";

    // look for the response command in the uart responseBuffer
    // format: +MQTTSUBRECV:0,"topic",5,Hallo
    uartToESP_Read(cmd, metaDataBuf, MAX_TOPIC_NAME_LENGTH + DIGITS_OF_LENGTH + 1);

    //Find the Topic attribute
    char *endOfTopic = strstr(metaDataBuf, "\",");
    int lengthOfTopic = endOfTopic - metaDataBuf;
    if (endOfTopic == NULL || lengthOfTopic > MAX_TOPIC_NAME_LENGTH - 1) {
        PRINT("Topic name too long")
        return false;
    }

    // copy Topic to the Message struct
    char *topicBuffer = malloc(sizeof(char) * (lengthOfTopic + 1));
    strncpy(topicBuffer, metaDataBuf, lengthOfTopic);
    topicBuffer[lengthOfTopic] = 0;
    response->topic = topicBuffer;

    // Find the length of Data attribute
    char *endOfLength = strstr(endOfTopic + 2, ",");
    int lengthOfLength = endOfLength - (endOfTopic + 2);
    if (endOfLength == NULL || lengthOfLength > DIGITS_OF_LENGTH - 1) {
        PRINT("Length has too many digits")
        return false;
    }

    // convert length of the data to int
    char dataLengthBuf[DIGITS_OF_LENGTH];
    strncpy(dataLengthBuf, endOfTopic + 2, lengthOfLength);
    dataLengthBuf[lengthOfLength] = 0;
    int dataLength = strtol(dataLengthBuf, NULL, 10);
    //PRINT("Got %d bytes", dataLength) // Debug

    // allocate memory for the response data and copy
    char *startAtString = malloc(sizeof(char) * (strlen(cmd) + lengthOfTopic + 2 + lengthOfLength + 1 + 1));
    sprintf(startAtString, "%s%s\",%i,", cmd, topicBuffer, dataLength);
    char *dataBuffer = malloc(sizeof(char) * (dataLength + 1));
    bool returnValue = false;
    if (uartToESP_Cut(startAtString, dataBuffer, dataLength)) {
        response->data = dataBuffer;
        returnValue = true;
    } else {
        PRINT("response got lost during processing (possibly due to another transmission via uart)")
    }
    free(startAtString);
    return returnValue;
}

void MQTT_Broker_SetClientId(char *clientId) {
//    ASSERT(NetworkStatus.ChipStatus)
    char cmd[100];
    strcpy(cmd, "AT+MQTTUSERCFG=0,1,\"");
    strcat(cmd, clientId);
    strcat(cmd, "\",\"\",\"\",0,0,\"\"");
    if (!ESP_SendCommand(cmd, "OK", 1000)) {
        PRINT("Could not set client id to %s, aborting...", clientId)
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

    ESP_SendCommand("AT+MQTTCLEAN=0", "OK", 5000);
    NetworkStatus.MQTTStatus = NOT_CONNECTED;
}

_Noreturn void MQTT_Broker_ReceiverTask() {
    MQTT_Broker_ReceiverTaskRegistered = true;
    Posting posting = {};
    while (true) {
        if (MQTT_Broker_IsResponseAvailable()) {
            if (MQTT_Broker_GetResponse(&posting)) {
                for (int i = 0; i < MQTT_Broker_numberSubscriber; ++i) {
                    if (MQTT_Broker_checkIfTopicMatches(MQTT_Broker_subscriberList[i].topic, posting.topic)) {
                        MQTT_Broker_subscriberList[i].subscriber.deliver(posting);
                    }
                }
                free(posting.topic);
                free(posting.data);
            }
        }
        TaskSleep(100);
    }
}

bool MQTT_Broker_topicsAreEqual(char *subscribedTopic, char *publishedTopic) {
    return strcmp(subscribedTopic, publishedTopic) == 0;
}

bool MQTT_Broker_subscribedTopicHasCharactersLeft(char *subscribedTopic, int subscribedIterator) {
    return subscribedIterator < strlen(subscribedTopic) && subscribedTopic[subscribedIterator] != '+';
}

bool MQTT_Broker_multilevelWildcardIn(const char *subscribedTopic, int subscribedIterator) {
    return subscribedTopic[subscribedIterator] == '#';
}

bool
MQTT_Broker_checkForEqualCharacterIn(const char *subscribedTopic, const char *publishedTopic, int subscribedIterator,
                                     int publishedIterator) {
    return subscribedTopic[subscribedIterator] == publishedTopic[publishedIterator];
}

bool MQTT_Broker_checkForSingleLevelWildcardIn(const char *subscribedTopic, int subscribedIterator) {
    return subscribedTopic[subscribedIterator] == '+';
}

bool MQTT_Broker_checkIfWildcardEndedIn(const char *publishedTopic, int publishedIterator) {
    return publishedTopic[publishedIterator] == '/';
}

bool MQTT_Broker_checkIfTopicMatches(char *subscribedTopic, char *publishedTopic) {
    if (MQTT_Broker_topicsAreEqual(subscribedTopic, publishedTopic)) {
        return true;
    }
    int subscribedIterator = 0;
    int publishedIterator = 0;

    while (publishedIterator < strlen(publishedTopic)) {
        if (MQTT_Broker_multilevelWildcardIn(subscribedTopic, subscribedIterator)) {
            return true;
        }
        if (MQTT_Broker_checkForEqualCharacterIn(subscribedTopic, publishedTopic, subscribedIterator,
                                                 publishedIterator)) {
            subscribedIterator++;
            publishedIterator++;
        } else if (MQTT_Broker_checkForSingleLevelWildcardIn(subscribedTopic, subscribedIterator)) {
            publishedIterator++;
            if (MQTT_Broker_checkIfWildcardEndedIn(publishedTopic, publishedIterator)) {
                subscribedIterator++;
            }
        } else {
            return false;
        }
    }
    if (MQTT_Broker_subscribedTopicHasCharactersLeft(subscribedTopic, subscribedIterator)) {
        return false;
    }
    return true;
}
