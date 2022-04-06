#define SOURCE_FILE "MQTT-Broker"

#include "espMQTTBroker.h"

#include "posting.h"
#include "protocol.h"
#include "communicationEndpoint.h"

#include "common.h"
#include "espBase.h"

#include <string.h>
#include <stdlib.h>

#include "TaskWrapper.h"

char *brokerDomain = "";
bool MQTTReceiverTaskRegistered = false;
uint numberSubscriber = 0;
Subscription subscriberList[MAX_SUBSCRIBER];

_Noreturn void ESP_MQTT_BROKER_INTERN_ReceiverTask();

bool checkIfTopicMatches(char *subscribedTopic, char *publishedTopic);

void setID(char *ID) {
    brokerDomain = ID;
}

char *concatIDWithTopic(const char *topic) {
    char *result = malloc(strlen(brokerDomain) + strlen(topic) + 1);
    strcpy(result, brokerDomain);
    strcat(result, "/");
    strcat(result, topic);
    return result;
}

void publish(Posting posting) {
    if (!MQTT_connected())
        return;

    char *topic = concatIDWithTopic(posting.topic);

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
    if (!MQTT_connected())
        return;
    subscribeRaw(concatIDWithTopic(topic), subscriber);

}

void unsubscribe(char *topic, Subscriber subscriber) {
    if (!MQTT_connected())
        return;
    unsubscribeRaw(concatIDWithTopic(topic), subscriber);
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
        if (numberSubscriber != MAX_SUBSCRIBER) {
            subscriberList[numberSubscriber] = (Subscription) {.topic=topic, .subscriber=subscriber};
            numberSubscriber++;
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
        for (int i = 0; i < numberSubscriber; ++i) {
            if (strcmp(subscriberList[i].topic, topic) == 0) {
                if (subscriberList[i].subscriber.deliver == subscriber.deliver) {
                    strcpy(subscriberList[i].topic, subscriberList[numberSubscriber].topic);
                    subscriberList[i].subscriber = subscriberList[numberSubscriber].subscriber;
                    strcpy(subscriberList[numberSubscriber].topic, "\0");
                    numberSubscriber--;
                }
            }
        }
        PRINT("Unsubscribed from %s.", topic)
    }
}

char *ID() {
    return brokerDomain;
}

bool ESP_MQTT_IsResponseAvailable() {
    return uartToESP_ResponseArrived("+MQTTSUBRECV:0,");
}

static bool ESP_MQTT_CutResponseToBuffer(Posting *response);

bool ESP_MQTT_GetResponse(Posting *response) {
    ASSERT(ESP_MQTT_IsResponseAvailable())
    return ESP_MQTT_CutResponseToBuffer(response);
}

bool ESP_MQTT_CutResponseToBuffer(Posting *response) {
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

void ESP_MQTT_BROKER_SetClientId(char *clientId) {
//    ASSERT(ESPChipStatusFlags.ChipStatus)
    char cmd[100];
    strcpy(cmd, "AT+MQTTUSERCFG=0,1,\"");
    strcat(cmd, clientId);
    strcat(cmd, "\",\"\",\"\",0,0,\"\"");
    if (!ESP_SendCommand(cmd, "OK", 1000)) {
        PRINT("Could not set client id to %s, aborting...", clientId)
    }
}

void ESP_MQTT_BROKER_ConnectToBroker(char *target, char *port) {
    if (ESPChipStatusFlags.ChipStatus && ESPChipStatusFlags.WIFIStatus == CONNECTED) {
        if (ESPChipStatusFlags.MQTTStatus == CONNECTED) {
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
            ESPChipStatusFlags.MQTTStatus = CONNECTED;
            if (!MQTTReceiverTaskRegistered)
                RegisterTask(ESP_MQTT_BROKER_INTERN_ReceiverTask, "ESP_MQTT_BROKER_INTERN_ReceiverTask");
        } else {
            PRINT("Could not connect to %s at Port %s", target, port)
        }
//    return ESPChipStatusFlags.MQTTStatus;
    } else {
        PRINT("Could not connect to MQTT broker. No Wifi connection or chip problem.")
    }
}

void ESP_MQTT_BROKER_Disconnect(bool force) {
    if (!force) {
        if (ESPChipStatusFlags.ChipStatus && ESPChipStatusFlags.WIFIStatus == CONNECTED) {
            if (ESPChipStatusFlags.MQTTStatus == NOT_CONNECTED) {
                PRINT("No connection to close")
                return;
            }
        }
    }

    ESP_SendCommand("AT+MQTTCLEAN=0", "OK", 5000);
    ESPChipStatusFlags.MQTTStatus = NOT_CONNECTED;
}

_Noreturn void ESP_MQTT_BROKER_INTERN_ReceiverTask() {
    MQTTReceiverTaskRegistered = true;
    Posting posting = {};
    while (true) {
        if (ESP_MQTT_IsResponseAvailable()) {
            if (ESP_MQTT_GetResponse(&posting)) {
                for (int i = 0; i < numberSubscriber; ++i) {
                    if (checkIfTopicMatches(subscriberList[i].topic, posting.topic)) {
                        subscriberList[i].subscriber.deliver(posting);
                    }
                }
                free(posting.topic);
                free(posting.data);
            }
        }
        TaskSleep(100);
    }
}

bool topicsAreEqual(char *subscribedTopic, char *publishedTopic) {
    return strcmp(subscribedTopic, publishedTopic) == 0;
}

bool subscribedTopicHasCharactersLeft(char *subscribedTopic, int subscribedIterator) {
    return subscribedIterator < strlen(subscribedTopic) && subscribedTopic[subscribedIterator] != '+';
}

bool multilevelWildcardIn(const char *subscribedTopic, int subscribedIterator) {
    return subscribedTopic[subscribedIterator] == '#';
}

bool checkForEqualCharacterIn(const char *subscribedTopic, const char *publishedTopic, int subscribedIterator,
                              int publishedIterator) {
    return subscribedTopic[subscribedIterator] == publishedTopic[publishedIterator];
}

bool checkForSingleLevelWildcardIn(const char *subscribedTopic, int subscribedIterator) {
    return subscribedTopic[subscribedIterator] == '+';
}

bool checkIfWildcardEndedIn(const char *publishedTopic, int publishedIterator) {
    return publishedTopic[publishedIterator] == '/';
}

bool checkIfTopicMatches(char *subscribedTopic, char *publishedTopic) {
    if (topicsAreEqual(subscribedTopic, publishedTopic)) {
        return true;
    }
    int subscribedIterator = 0;
    int publishedIterator = 0;

    while (publishedIterator < strlen(publishedTopic)) {
        if (multilevelWildcardIn(subscribedTopic, subscribedIterator)) {
            return true;
        }
        if (checkForEqualCharacterIn(subscribedTopic, publishedTopic, subscribedIterator, publishedIterator)) {
            subscribedIterator++;
            publishedIterator++;
        } else if (checkForSingleLevelWildcardIn(subscribedTopic, subscribedIterator)) {
            publishedIterator++;
            if (checkIfWildcardEndedIn(publishedTopic, publishedIterator)) {
                subscribedIterator++;
            }
        } else {
            return false;
        }
    }
    if (subscribedTopicHasCharactersLeft(subscribedTopic, subscribedIterator)) {
        return false;
    }
    return true;
}
