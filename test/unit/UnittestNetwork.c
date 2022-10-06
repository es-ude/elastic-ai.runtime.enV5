#include "Esp.h"
#include "Network.h"
#include "esp/EspUnitTest.h"
#include "unity.h"

NetworkCredentials_t credentials = {.ssid = "SSID", .password = "password"};

void setUp(void) {
    ESP_Status.ChipStatus = ESP_CHIP_OK;
    ESP_Status.WIFIStatus = NOT_CONNECTED;
    ESP_Status.MQTTStatus = NOT_CONNECTED;

    esp_setErrorCode(ESP_NO_ERROR);
}

void tearDown(void) {}

/* region Test Functions */

void TEST_CONNECT_TO_NETWORK_SUCCESSFUL(void) {
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    network_errorCode networkErrorCode = network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(NETWORK_NO_ERROR, networkErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
}
void TEST_CONNECT_TO_NETWORK_ALREADY_CONNECTED(void) {
    ESP_Status.WIFIStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    network_errorCode networkErrorCode = network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(NETWORK_WIFI_ALREADY_CONNECTED, networkErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
}
void TEST_CONNECT_TO_NETWORK_ESP_CHIP_FAILED(void) {
    ESP_Status.ChipStatus = ESP_CHIP_NOT_OK;
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    TEST_ASSERT_EQUAL(ESP_CHIP_NOT_OK, ESP_Status.ChipStatus);
    network_errorCode networkErrorCode = network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(NETWORK_ESP_CHIP_FAILED, networkErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
}
void TEST_CONNECT_TO_NETWORK_SEND_FAILED(void) {
    esp_setErrorCode(ESP_WRONG_ANSWER_RECEIVED);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    network_errorCode networkErrorCode = network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(NETWORK_ESTABLISH_CONNECTION_FAILED, networkErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
}

void TEST_DISCONNECT_FROM_NETWORK(void) {
    ESP_Status.WIFIStatus = CONNECTED;
    ESP_Status.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.MQTTStatus);
    network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
}
void TEST_DISCONNECT_FROM_NETWORK_TWICE(void) {
    ESP_Status.WIFIStatus = CONNECTED;
    ESP_Status.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.MQTTStatus);
    network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
    network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(TEST_CONNECT_TO_NETWORK_SUCCESSFUL);
    RUN_TEST(TEST_CONNECT_TO_NETWORK_ALREADY_CONNECTED);
    RUN_TEST(TEST_CONNECT_TO_NETWORK_ESP_CHIP_FAILED);
    RUN_TEST(TEST_CONNECT_TO_NETWORK_SEND_FAILED);

    RUN_TEST(TEST_DISCONNECT_FROM_NETWORK);
    RUN_TEST(TEST_DISCONNECT_FROM_NETWORK_TWICE);

    return UNITY_END();
}
