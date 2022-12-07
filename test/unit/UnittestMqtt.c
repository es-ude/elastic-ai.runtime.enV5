#include "Esp.h"
#include "MqttBroker.h"
#include "esp/EspUnitTest.h"
#include <unity.h>

mqttBrokerHost_t mqttHost = {
    .ip = "127.0.0.1", .port = "1883", .userID = "user", .password = "password"};

void setUp(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    espStatus.MQTTStatus = NOT_CONNECTED;

    ESPDUMMY_RETURN_CODE = ESP_NO_ERROR;
}

void tearDown(void) {}

/* region Test Functions */

void testConnectToMqttBrokerSuccessful(void) {
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
    mqttBrokerErrorCode_t mqttErrorCode =
        mqttBrokerConnectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_NO_ERROR, mqttErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
}
void testConnectToMqttBrokerEspFailed(void) {
    espStatus.ChipStatus = ESP_CHIP_NOT_OK;
    TEST_ASSERT_EQUAL(ESP_CHIP_NOT_OK, espStatus.ChipStatus);
    mqttBrokerErrorCode_t mqttErrorCode =
        mqttBrokerConnectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_ESP_CHIP_FAILED, mqttErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}
void testConnectToMqttBrokerNoWifi(void) {
    espStatus.WIFIStatus = NOT_CONNECTED;
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    mqttBrokerErrorCode_t mqttErrorCode =
        mqttBrokerConnectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_WIFI_FAILED, mqttErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}
void testConnectToMqttBrokerAlreadyConnected(void) {
    espStatus.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
    mqttBrokerErrorCode_t mqttErrorCode =
        mqttBrokerConnectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_ALREADY_CONNECTED, mqttErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
}
void testConnectToMqttBrokerSendCommandFailed(void) {
    ESPDUMMY_RETURN_CODE = ESP_WRONG_ANSWER_RECEIVED;
    mqttBrokerErrorCode_t mqttErrorCode =
        mqttBrokerConnectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_ESP_WRONG_ANSWER, mqttErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testConnectToMqttBrokerSuccessful);
    RUN_TEST(testConnectToMqttBrokerEspFailed);
    RUN_TEST(testConnectToMqttBrokerNoWifi);
    RUN_TEST(testConnectToMqttBrokerAlreadyConnected);
    RUN_TEST(testConnectToMqttBrokerSendCommandFailed);

    return UNITY_END();
}
