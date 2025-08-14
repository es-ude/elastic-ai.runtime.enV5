#define SOURCE_FILE "HARDWARETEST-DURABILITY"

/*!
 * Connects to Wi-Fi and MQTT Broker.
 * Subscribes to START/STOP topic of "wifi" and "sram".
 * Responds to data requests for these topics!
 */

// external headers
#include <malloc.h>
#include <string.h>

// pico-sdk
#include <hardware/i2c.h>

// src headers
#include "HardwareTestHelper.h"
#include "Protocol.h"
#include "eai/Common.h"
#include "eai/network/Esp.h"
#include "eai/network/MqttBroker.h"
#include "eai/rtos/FreeRtosQueueWrapper.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"
#include "eai/sensor/Pac193x.h"

typedef enum topics {
    WIFI = 0,
    SRAM = 1,
} topics_t;
char *topic_strings[] = {"wifi", "sram"};

typedef struct publishRequest {
    topics_t topic;
    char *data;
} publishRequest_t;

typedef struct dataRequester {
    char *clientId;
    bool wifiSubscribed;
    bool sramSubscribed;
    struct dataRequester *nextItem;
} dataRequester_t;

typedef struct countUpdateMessage {
    topics_t topic;
    bool add;
} countUpdateMessage_t;

queue_t postingQueue;
queue_t updateSubscriberCount;
queue_t publishData;

static pac193xSensorConfiguration_t sensor1 = {
    .i2c_host = i2c1,
    .i2c_slave_address = PAC193X_I2C_ADDRESS_499R,
    .powerPin = -1,
    .usedChannels = {.uint_channelsInUse = 0b00001111},
    .rSense = {0.82f, 0.82f, 0.82f, 0.82f},
};
#define PAC193X_CHANNEL_WIFI PAC193X_CHANNEL04

static pac193xSensorConfiguration_t sensor2 = {
    .i2c_host = i2c1,
    .i2c_slave_address = PAC193X_I2C_ADDRESS_806R,
    .powerPin = -1,
    .usedChannels = {.uint_channelsInUse = 0b00001111},
    .rSense = {0.82f, 0.82f, 0.82f, 0.82f},
};
#define PAC193X_CHANNEL_FPGA_SRAM PAC193X_CHANNEL04

void receivePosting(posting_t posting) {
    freeRtosQueueWrapperPushFromInterrupt(postingQueue, &posting);
}
void makeDataAvailable(char *dataTopic) {
    protocolSubscribeForDataStartRequest(dataTopic, (subscriber_t){.deliver = receivePosting});
    protocolSubscribeForDataStopRequest(dataTopic, (subscriber_t){.deliver = receivePosting});
}
void addClientToList(dataRequester_t **list, char *clientId, topics_t topic) {
    dataRequester_t *current = *list;
    while (current != NULL) {
        if (0 == strcmp(current->clientId, clientId)) {
            if (WIFI == topic && !current->wifiSubscribed) {
                countUpdateMessage_t msg = {.topic = WIFI, .add = true};
                freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
                current->wifiSubscribed = true;
            } else if (SRAM == topic && !current->sramSubscribed) {
                countUpdateMessage_t msg = {.topic = SRAM, .add = true};
                freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
                current->sramSubscribed = true;
            }
            return;
        }
        if (current->nextItem == NULL) {
            break;
        }
    }

    protocolSubscribeForStatus(clientId, (subscriber_t){.deliver = receivePosting});
    dataRequester_t *newSubscriber = calloc(1, sizeof(dataRequester_t));
    newSubscriber->clientId = clientId;
    switch (topic) {
    case WIFI:
        newSubscriber->wifiSubscribed = true;
        break;
    case SRAM:
        newSubscriber->sramSubscribed = true;
        break;
    }
    countUpdateMessage_t msg = {.topic = topic, .add = true};
    freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
    newSubscriber->nextItem = NULL;

    if (current == NULL) {
        *list = newSubscriber;
    } else {
        current->nextItem = newSubscriber;
    }
}
void removeClientFromList(dataRequester_t **list, char *clientId, topics_t topic) {
    if (*list == NULL) {
        return;
    }
    dataRequester_t *last = NULL;
    dataRequester_t *current = *list;
    if (0 == strstr(current->clientId, clientId)) {
        if (topic == WIFI && current->wifiSubscribed) {
            countUpdateMessage_t msg = {.topic = WIFI, .add = false};
            freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
            current->wifiSubscribed = false;
        }
        if (topic == SRAM && current->sramSubscribed) {
            countUpdateMessage_t msg = {.topic = SRAM, .add = false};
            freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
            current->sramSubscribed = false;
        }
        if (!current->wifiSubscribed && !current->sramSubscribed) {
            protocolUnsubscribeFromStatus(clientId, (subscriber_t){.deliver = receivePosting});
            *list = current->nextItem;
            free(current->clientId);
            free(current);
        }
        return;
    }
    last = current;
    current = current->nextItem;
    while (current != NULL) {
        if (0 == strstr(current->clientId, clientId)) {
            if (topic == WIFI && current->wifiSubscribed) {
                countUpdateMessage_t msg = {.topic = WIFI, .add = false};
                freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
                current->wifiSubscribed = false;
            }
            if (topic == SRAM && current->sramSubscribed) {
                countUpdateMessage_t msg = {.topic = SRAM, .add = false};
                freeRtosQueueWrapperPush(updateSubscriberCount, &msg);
                current->sramSubscribed = false;
            }
            if (!current->wifiSubscribed && !current->sramSubscribed) {
                protocolUnsubscribeFromStatus(clientId, (subscriber_t){.deliver = receivePosting});
                last->nextItem = current->nextItem;
                free(current->clientId);
                free(current);
            }
            return;
        }
        last = current;
        current = current->nextItem;
    }
}
void handlePosting(posting_t *posting, dataRequester_t **subscriberList) {
    if (strstr(posting->topic, START)) {
        addClientToList(subscriberList, posting->data,
                        strstr(posting->topic, "wifi") ? WIFI : SRAM);
        free(posting->topic);
    } else if (strstr(posting->topic, STOP)) {
        removeClientFromList(subscriberList, posting->data,
                             strstr(posting->topic, "wifi") ? WIFI : SRAM);
    } else if (strstr(posting->data, STATUS_STATE_OFFLINE)) {
        posting->topic[strlen(posting->topic) - 7] = '\0';
        removeClientFromList(subscriberList, posting->topic, WIFI);
        removeClientFromList(subscriberList, posting->topic, SRAM);
        free(posting->topic);
        free(posting->data);
    } else {
        PRINT("Received unknown message:\n{\n\ttopic: %s\n\tcontent: %s\n}", posting->topic,
              posting->data);
    }
}
_Noreturn void networkManagerTask(void) {
    dataRequester_t *subscriberList = NULL;

    makeDataAvailable("sram");
    makeDataAvailable("wifi");
    publishAliveStatusMessageWithMandatoryAttributes(
        (status_t){.type = "enV5", .state = STATUS_STATE_ONLINE, .data = "wifi,sram"});

    while (1) {
        posting_t posting;
        if (freeRtosQueueWrapperPop(postingQueue, &posting)) {
            handlePosting(&posting, &subscriberList);
        }

        publishRequest_t publishRequest;
        if (freeRtosQueueWrapperPop(publishData, &publishRequest)) {
            protocolPublishData(topic_strings[publishRequest.topic], publishRequest.data);
            free(publishRequest.data);
        }
    }
}

void updateCounter(countUpdateMessage_t *msg, uint16_t *counter) {
    if (msg->add) {
        *counter += 1;
    } else {
        *counter -= 1;
    }
    if (*counter < 0) {
        *counter = 0;
    }
}
float measureValue(pac193xSensorConfiguration_t sensor, pac193xChannel_t channel) {
    float measurement;

    pac193xErrorCode_t errorCode =
        pac193xGetMeasurementForChannel(sensor, channel, PAC193X_VSOURCE_AVG, &measurement);
    if (errorCode != PAC193X_NO_ERROR) {
        PRINT("  \033[0;31mFAILED\033[0m; pac193x_ERROR: %02X", errorCode);
        return -1;
    }
    return measurement;
}
void publishWifiUpdate(void) {
    char *buffer = calloc(64, sizeof(char));
    float sensorValue = measureValue(sensor1, PAC193X_CHANNEL_WIFI);
    if (sensorValue >= 0) {
        snprintf(buffer, 64, "%f", sensorValue);
        publishRequest_t pubRequest = {.topic = WIFI, .data = buffer};
        freeRtosQueueWrapperPush(publishData, &pubRequest);
    }
}
void publishSramUpdate(void) {
    char *buffer = calloc(64, sizeof(char));
    float sensorValue = measureValue(sensor2, PAC193X_CHANNEL_FPGA_SRAM);
    if (sensorValue >= 0) {
        snprintf(buffer, 64, "%f", sensorValue);
        publishRequest_t pubRequest = {.topic = SRAM, .data = buffer};
        freeRtosQueueWrapperPush(publishData, &pubRequest);
    }
}
_Noreturn void sensorTask(void) {
    uint16_t wifiSubscriber = 0;
    uint16_t sramSubscriber = 0;

    while (1) {
        countUpdateMessage_t countUpdateMessage;
        if (freeRtosQueueWrapperPop(updateSubscriberCount, &countUpdateMessage)) {
            if (countUpdateMessage.topic == WIFI) {
                updateCounter(&countUpdateMessage, &wifiSubscriber);
            } else {
                updateCounter(&countUpdateMessage, &sramSubscriber);
            }
        }

        if (wifiSubscriber) {
            publishWifiUpdate();
        }
        if (sramSubscriber) {
            publishSramUpdate();
        }
    }
}

void initSensors(void) {
    PRINT("===== INIT SENSOR 1 =====");
    pac193xErrorCode_t errorCode;
    while (1) {
        errorCode = pac193xInit(sensor1);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.\n");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }

    PRINT("===== INIT SENSOR 2 =====");
    while (1) {
        errorCode = pac193xInit(sensor2);
        if (errorCode == PAC193X_NO_ERROR) {
            PRINT("Initialised PAC193X sensor 1.\n");
            break;
        }
        PRINT("Initialise PAC193X failed; pac193x_ERROR: %02X\n", errorCode);
        sleep_ms(500);
    }
}

int main() {
    initHardwareTest();
    connectToNetwork();
    connectToMqttBroker();
    initSensors();

    postingQueue = freeRtosQueueWrapperCreate(10, sizeof(posting_t));
    updateSubscriberCount = freeRtosQueueWrapperCreate(20, sizeof(countUpdateMessage_t));
    publishData = freeRtosQueueWrapperCreate(20, sizeof(publishRequest_t));

    freeRtosTaskWrapperRegisterTask(enterBootModeTask, "enterBootModeTask", 30, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(networkManagerTask, "subscriberManager", 0, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(sensorTask, "sensorTask", 0, FREERTOS_CORE_1);
    freeRtosTaskWrapperStartScheduler();
}
