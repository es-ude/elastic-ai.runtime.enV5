#define SOURCE_FILE "MODEL-STORE-TEST"

#include "MQTTBroker.h"
#include "TaskWrapper.h"
#include "common.h"
#include "hardwareTestHelper.h"
#include "modelStore.h"
#include "protocol.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

/*!
 * Connects to the model store and Searches for suitable model example model, if sucessfull
 * tires to get it over http
 */

extern MQTTHost_t MQTTHost;

char *modelStoreIP = "192.168.203.46:5000";
char *modelSearch = "{"
                    "\"@id\": \"http:///problem_description\","
                    "\"http://%s/service_namespace#MeanAbsoluteError\": 0.3,"
                    "\"http://%s/service_namespace#Input\": {"
                    "\"@id\": \"http://%s/service_namespace#Float\""
                    "},"
                    "\"http://%s/service_namespace#Output\": {"
                    "\"@id\": \"http://%s/service_namespace#Float\""
                    "},"
                    "\"http://%s/service_namespace#Predict\": {"
                    "\"@id\": \"http://%s/service_namespace#Sine\""
                    "},"
                    "\"http://%s/service_namespace#Size\": 2500"
                    "}";

void _Noreturn modelTask(void) {

    char *problem_graph = malloc(strlen(modelSearch) + 8 * strlen(modelStoreIP));
    sprintf(problem_graph, modelSearch, modelStoreIP, modelStoreIP, modelStoreIP, modelStoreIP,
            modelStoreIP, modelStoreIP, modelStoreIP, modelStoreIP);

    connectToNetwork();
    connectToMQTT();

    while (true) {
        PRINT("=== STARTING TEST ===")

        ModelStore_connect("1");
        TaskSleep(3000);

        char *modelUri;
        ModelStoreStatus status = ModelStore_searchModel(problem_graph, &modelUri);
        TaskSleep(3000);

        if (status == MS_SUCCESS) {
            PRINT("%s", modelUri)

            TaskSleep(3000);

            char *modelData;
            status = ModelStore_getModel(modelUri, &modelData);
            if (status == MS_SUCCESS) {
                PRINT("Received model data")
            } else {
                PRINT("Error in the model store: %d", status)
            }
        } else {
            PRINT("Error in the model store: %d", status)
        }

        TaskSleep(3000);
        ModelStore_disconnect();
        TaskSleep(3000);
        PRINT("=== END TEST ===\n")
        TaskSleep(3000);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(modelTask, "modelTask");
    StartScheduler();
}
