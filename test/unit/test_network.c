#define TEST_BUILD

#include "Network.h"
#include "esp/esp_test.h"
#include "unity.h"

NetworkCredentials_t credentials = {.ssid = "SSID", .password = "password"};

void setUp(void) { ESP_ReturnTrue(); }

void tearDown(void) { TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.MQTTStatus); }

void TEST_NETWORK_INIT(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
}

void TEST_CONNECT_TO_NETWORK(void) {
    Network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    Network_DisconnectFromNetwork();
}

void TEST_CONNECT_TO_NETWORK_TWICE(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    Network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    Network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    Network_DisconnectFromNetwork();
}

void TEST_CONNECT_TO_NETWORK_FAIL(void) {
    ESP_ReturnFalse();
    Network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    Network_DisconnectFromNetwork();
}

void TEST_DISCONNECT_FROM_NETWORK(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    Network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    Network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
}

void TEST_DISCONNECT_FROM_NETWORK_TWICE(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, ESP_Status.ChipStatus);
    Network_ConnectToNetwork(credentials);
    TEST_ASSERT_EQUAL(CONNECTED, ESP_Status.WIFIStatus);
    Network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
    Network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(NOT_CONNECTED, ESP_Status.WIFIStatus);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(TEST_NETWORK_INIT);

    RUN_TEST(TEST_CONNECT_TO_NETWORK);
    RUN_TEST(TEST_CONNECT_TO_NETWORK_TWICE);
    RUN_TEST(TEST_CONNECT_TO_NETWORK_FAIL);

    RUN_TEST(TEST_DISCONNECT_FROM_NETWORK);
    RUN_TEST(TEST_DISCONNECT_FROM_NETWORK_TWICE);

    return UNITY_END();
}
