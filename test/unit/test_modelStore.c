#define TEST_BUILD

#include "MQTTBroker.h"
#include "modelStore.h"

#include "network_configuration.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_ModelStore_getModel() {

    mqtt_connectToBroker(
        (MQTTHost_t){.ip = "127.0.0.1", .port = "1883", .userID = "", .password = ""}, "", "");
    ModelStore_connect("5");

    char *model;
    ModelStoreStatus status = ModelStore_getModel(
        "model:c67f1c6e5b93d5ee9d9948146357f68c0b28f39f572215f81c191dabda429e10", &model);
    TEST_ASSERT_EQUAL(MS_SUCCESS, status);

    ModelStore_disconnect();
}

void test_ModelStore_searchModel() {
    const char *problem_graph =
        "{"
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

    mqtt_connectToBroker(MQTTHost, "", "");
    ModelStore_connect("5");

    char *modelUri;
    ModelStoreStatus status = ModelStore_searchModel(problem_graph, &modelUri);
    TEST_ASSERT_EQUAL(MS_SUCCESS, status);

    ModelStore_disconnect();
}

void test_ModelStore_searchModel_modelUriNotFound() {
    const char *problem_graph = "{"
                                "\"@id\": \"http://platzhalter.de/problem_description\","
                                "\"http://platzhalter.de/service_namespace#MeanAbsoluteError\": 1,"
                                "\"http://platzhalter.de/service_namespace#Input\": {"
                                "\"@id\": \"http://platzhalter.de/service_namespace#Float\""
                                "},"
                                "\"http://platzhalter.de/service_namespace#Output\": {"
                                "\"@id\": \"http://platzhalter.de/service_namespace#Float\""
                                "},"
                                "\"http://platzhalter.de/service_namespace#Predict\": {"
                                "\"@id\": \"http://platzhalter.de/service_namespace#Sine\""
                                "},"
                                "\"http://platzhalter.de/service_namespace#Size\": 1"
                                "}";

    mqtt_connectToBroker(MQTTHost, "", "");
    ModelStore_connect("5");

    char *modelUri;
    ModelStoreStatus status = ModelStore_searchModel(problem_graph, &modelUri);
    TEST_ASSERT_EQUAL(MS_MODEL_URI_NOT_FOUND, status);

    ModelStore_disconnect();
}

void test_ModelStore_getModel_invalidModelHash() {
    mqtt_connectToBroker(MQTTHost, "", "");
    ModelStore_connect("5");

    char *model;
    ModelStoreStatus status = ModelStore_getModel(
        // b"invalid hash".ljust(32, b"-")
        "model:696e76616c696420686173682d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d", &model);
    TEST_ASSERT_EQUAL(MS_MODEL_DATA_NOT_FOUND, status);

    ModelStore_disconnect();
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ModelStore_getModel);
    RUN_TEST(test_ModelStore_getModel_invalidModelHash);
    RUN_TEST(test_ModelStore_searchModel);
    RUN_TEST(test_ModelStore_searchModel_modelUriNotFound);

    return UNITY_END();
}
