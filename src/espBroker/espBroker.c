#define SOURCE_FILE "Broker"

#include "espBroker.h"
#include "posting.h"
#include "protocol.h"

#include "communicationEndpoint.h"

#include "common.h"

#include "espBase.h"

#include "string.h"
#include "stdlib.h"

char *brokerDomain = "";

void setID(char *ID) {
    brokerDomain = ID;
}

char *concatIDWithTopic(const char *topic) {
    char *result = malloc(strlen(brokerDomain) + strlen(topic) + 1);
    strcpy(result, brokerDomain);
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
            strlen(posting.topic) +
            strlen(posting.data) + 1));
    sprintf(command, "%s%s%s%s%s", cmd1, topic, cmd2, posting.data, cmd3);

    if (!ESP_SendCommand(command, "OK", 1000)) {
        PRINT("Could not publish to topic: %s.", topic)
    } else {
        PRINT("Published to %s.", topic)
    }
    free(command);
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
    }
}

void unsubscribeRaw(char *topic, Subscriber subscriber) {
    char command[50];
    strcpy(command, "AT+MQTTUNSUB=0,\"");
    strcat(command, topic);
    strcat(command, "\"");

    if (!ESP_SendCommand(command, "OK", 1000)) {
        PRINT("Could not unsubscribe to topic: %s. Have you subscribed beforehand?", topic)
    }
}

char *ID() {}

bool ESP_MQTT_IsResponseAvailable() {
    return uartToESP_ResponseArrived("+MQTTSUBRECV:0,");
}

static void ESP_MQTT_CutResponseToBuffer(Posting *response);

void ESP_MQTT_GetResponse(Posting *response) {
    if (ESP_MQTT_IsResponseAvailable()) {
        ESP_MQTT_CutResponseToBuffer(response);
    }
}

static void ESP_MQTT_CutResponseToBuffer(Posting *response) {
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
        return;
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
        return;
    }

    // convert length of the data to int
    char dataLengthBuf[DIGITS_OF_LENGTH];
    strncpy(dataLengthBuf, endOfTopic + 2, lengthOfLength);
    dataLengthBuf[lengthOfLength] = 0;
    int dataLength = atoi(dataLengthBuf);
    //PRINT("Got %d bytes", dataLength) // Debug

    // allocate memory for the response data and copy
    char *startAtString = malloc(sizeof(char) * (strlen(cmd) + lengthOfTopic + 2 + lengthOfLength + 1 + 1));
    sprintf(startAtString, "%s%s\",%i,", cmd, topicBuffer, dataLength);
    char *responseBuf = malloc(sizeof(char) * (dataLength + 1));
    if (uartToESP_Cut(startAtString, responseBuf, dataLength)) {
        response->data = responseBuf;
        free(startAtString);
    } else {
        PRINT("response got lost during processing (possibly due to another transmission via uart) :(")
    }
}

void ESP_MQTT_SetClientId(char *clientId) {
//    ASSERT(ESPChipStatusFlags.ChipStatus)
    char cmd[100];
    strcpy(cmd, "AT+MQTTUSERCFG=0,1,\"");
    strcat(cmd, clientId);
    strcat(cmd, "\",\"\",\"\",0,0,\"\"");
    if (!ESP_SendCommand(cmd, "OK", 1000)) {
        PRINT("Could not set client id to %s, aborting...", clientId)
    }
}

void ESP_MQTT_ConnectToBroker(char *target, char *port) {
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
        if (ESP_SendCommand(cmd, "+MQTTCONNECTED", 10000)) {
            PRINT("Connected to %s at Port %s", target, port)
            ESPChipStatusFlags.MQTTStatus = CONNECTED;
        } else {
            PRINT("Could not connect to %s at Port %s", target, port)
        }
//    return ESPChipStatusFlags.MQTTStatus;
    } else {
        PRINT("Could not connect to MQTT broker. No Wifi connection or chip problem.")
    }
}

void ESP_MQTT_Disconnect(bool force) {
    if (!force) {
        if (ESPChipStatusFlags.ChipStatus && ESPChipStatusFlags.WIFIStatus == CONNECTED) {
            if (ESPChipStatusFlags.MQTTStatus == NOT_CONNECTED) {
                PRINT("No connection to close")
                return;
            }
        }
    }

    ESP_SendCommand("AT+MQTTCLEAN=0", "OK", 1000);
    ESPChipStatusFlags.MQTTStatus = NOT_CONNECTED;

//    return ESPChipStatusFlags.MQTTStatus;
}
