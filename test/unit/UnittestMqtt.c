#include "Esp.h"
#include "MqttBroker.h"
#include "esp/EspUnitTest.h"
#include "unity.h"

MQTTHost_t mqttHost = {.ip = "127.0.0.1", .port = "1883", .userID = "user", .password = "password"};

void setUp(void) {
    ESP_Status.ChipStatus = ESP_CHIP_OK;
    ESP_Status.WIFIStatus = CONNECTED;
    ESP_Status.MQTTStatus = NOT_CONNECTED;

    esp_setErrorCode(ESP_NO_ERROR);
}

void tearDown(void) {}

/* region Test Functions */

void TEST_CONNECT_TO_MQTT_BROKER_SUCCESSFUL(void) {
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
    mqtt_errorCode mqttErrorCode = mqtt_connectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_NO_ERROR, mqttErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.MQTTStatus);
}
void TEST_CONNECT_TO_MQTT_BROKER_ESP_FAILED(void) {
    ESP_Status.ChipStatus = ESP_CHIP_NOT_OK;
    TEST_ASSERT_EQUAL(ESP_CHIP_NOT_OK, ESP_Status.ChipStatus);
    mqtt_errorCode mqttErrorCode = mqtt_connectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_ESP_CHIP_FAILED, mqttErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
}
void TEST_CONNECT_TO_MQTT_BROKER_NO_WIFI(void) {
    ESP_Status.WIFIStatus = NOT_CONNECTED;
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    mqtt_errorCode mqttErrorCode = mqtt_connectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_WIFI_FAILED, mqttErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
}
void TEST_CONNECT_TO_MQTT_BROKER_ALREADY_CONNECTED(void) {
    ESP_Status.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.MQTTStatus);
    mqtt_errorCode mqttErrorCode = mqtt_connectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_ALREADY_CONNECTED, mqttErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.MQTTStatus);
}
void TEST_CONNECT_TO_MQTT_BROKER_SEND_COMMAND_FAILED(void) {
    esp_setErrorCode(ESP_WRONG_ANSWER_RECEIVED);
    mqtt_errorCode mqttErrorCode = mqtt_connectToBroker(mqttHost, "testBroker", "testClient");
    TEST_ASSERT_EQUAL(MQTT_ESP_WRONG_ANSWER, mqttErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(TEST_CONNECT_TO_MQTT_BROKER_SUCCESSFUL);
    RUN_TEST(TEST_CONNECT_TO_MQTT_BROKER_ESP_FAILED);
    RUN_TEST(TEST_CONNECT_TO_MQTT_BROKER_NO_WIFI);
    RUN_TEST(TEST_CONNECT_TO_MQTT_BROKER_ALREADY_CONNECTED);
    RUN_TEST(TEST_CONNECT_TO_MQTT_BROKER_SEND_COMMAND_FAILED);

    return UNITY_END();
}
