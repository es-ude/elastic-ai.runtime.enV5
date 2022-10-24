#define SOURCE_FILE "MODEL-STORE"

#include "modelStore.h"

#include <stdlib.h>
#include <string.h>

#include "MQTTBroker.h"
#include "SemaphoreWrapper.h"
#include "common.h"
#include "communicationEndpoint.h"
#include "esp.h"
#include "http.h"

char *ModelStore_clientId;
Subscriber ModelStore_subscriber = {.deliver = responseCallback};

Semaphore ModelStore_responseReady;
char *ModelStore_responseData;

void responseCallback(Posting posting) {
    free(posting.topic);

    ModelStore_responseData = posting.data;
    SemaphoreGiveFromISR(ModelStore_responseReady);
}

void ModelStore_connect(char *topic) {
    if (ESP_Status.MQTTStatus == NOT_CONNECTED) {
        return;
    }
    ModelStore_clientId = topic;
    ModelStore_responseReady = SemaphoreCreateBinary();
    subscribe(topic, ModelStore_subscriber);
}

void ModelStore_disconnect() {
    unsubscribe(ModelStore_clientId, ModelStore_subscriber);
    SemaphoreDelete(ModelStore_responseReady);
}

ModelStoreStatus ModelStore_searchModel(const char *problem_graph, char **modelUri) {
    char request[strlen(ModelStore_clientId) + 1 + strlen(problem_graph) + 1];
    sprintf(request, "%s$%s", ModelStore_clientId, problem_graph);

    publishLong((Posting){.topic = "service/searchModel", .data = request});

    if (!SemaphoreTake(ModelStore_responseReady, 3000)) {
        PRINT("Timeout in searchModel");
        return MS_TIMEOUT;
    }

    if (ModelStore_responseData[0] == '!') {
        return (ModelStoreStatus)strtol(ModelStore_responseData + 1, NULL, 10);
    }

    *modelUri = ModelStore_responseData;
    return MS_SUCCESS;
}

ModelStoreStatus ModelStore_getModel(const char *modelUri, char **model) {
    char request[strlen(ModelStore_clientId) + 1 + strlen(modelUri) + 1];
    sprintf(request, "%s$%s", ModelStore_clientId, modelUri);

    publish((Posting){.topic = "service/getModel", .data = request});

    if (!SemaphoreTake(ModelStore_responseReady, 1000)) {
        PRINT("Timeout in getModel(%s)", modelUri)
        return MS_TIMEOUT;
    }

    if (ModelStore_responseData[0] == '!') {
        return (ModelStoreStatus)strtol(ModelStore_responseData + 1, NULL, 10);
    }

    char *modelDataUrl = ModelStore_responseData;
    char *modelData;
    HTTPStatus status = HTTP_get(modelDataUrl, &modelData);
    if (status != HTTP_SUCCESS)
        return MS_DOWNLOAD_ERROR;

    *model = modelData;
    return MS_SUCCESS;
}
