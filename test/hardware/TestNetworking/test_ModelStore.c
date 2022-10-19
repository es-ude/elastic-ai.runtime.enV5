#define SOURCE_FILE "MQTT-PUBLISH-TEST"

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

 */

extern MQTTHost_t MQTTHost;

const char *problem_graph = "{"
                            "\"@id\": \"http://platzhalter.de/problem_description\","
                            "\"http://platzhalter.de/service_namespace#MeanAbsoluteError\": 0.3,"
                            "\"http://platzhalter.de/service_namespace#Input\": {"
                            "\"@id\": \"http://platzhalter.de/service_namespace#Float\""
                            "},"
                            "\"http://platzhalter.de/service_namespace#Output\": {"
                            "\"@id\": \"http://platzhalter.de/service_namespace#Float\""
                            "},"
                            "\"http://platzhalter.de/service_namespace#Predict\": {"
                            "\"@id\": \"http://platzhalter.de/service_namespace#Sine\""
                            "},"
                            "\"http://platzhalter.de/service_namespace#Size\": 2500"
                            "}";

void _Noreturn modelTask(void) {

    connectToNetwork();
    connectToMQTT();

    PRINT("=== STARTING TEST ===")

    while (true) {
        ModelStore_connect("1");
        TaskSleep(1000);

        char *modelUri;
        ModelStoreStatus status = ModelStore_searchModel(problem_graph, &modelUri);
        TaskSleep(1000);

        if (status == MS_SUCCESS) {
            PRINT("%s", modelUri)
        } else {
            PRINT("Error in the model store: %d", status)
        }

        char *modelData;
        status = ModelStore_getModel(modelUri, &modelData);
        if (status == MS_SUCCESS) {
            PRINT("Received model data")
        } else {
            PRINT("Error in the model store: %d", status)
        }
        TaskSleep(3000);
    }
}

int main() {
    initHardwareTest();
    RegisterTask(enterBootModeTaskHardwareTest, "enterBootModeTask");
    RegisterTask(modelTask, "modelTask");
    StartScheduler();
}
