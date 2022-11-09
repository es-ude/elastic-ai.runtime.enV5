#define TEST_BUILD


#include "unity.h"

#include "ModelStore.h"
#include "MqttBroker.h"
#include "NetworkConfiguration.h"
#include "Esp.h"


/**
 * This is does not work. These are not unit tests, as they do not test a single unit. This comes with the problem, that certain global variables are not set,
 * because are are set after esp/network/etc. connection. There is no simple solution to it. So either, the ModelStore Unit tests are not existing, only the
 * integration test. Or the entire modelStore library gets refactored in order to provide unit tests up to a certain point.
 */

void setUp(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    espStatus.MQTTStatus = CONNECTED;
}

void tearDown(void) {}

void test_ModelStore_getModel() {
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

    ModelStore_connect("5");
    
    char *modelUri;
    ModelStoreStatus status = ModelStore_searchModel(problem_graph, &modelUri);
    TEST_ASSERT_EQUAL(MS_MODEL_URI_NOT_FOUND, status);
    
    ModelStore_disconnect();
  }

void test_ModelStore_getModel_invalidModelHash() {
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