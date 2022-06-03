#define TEST_BUILD

#include "esp/esp_test.h"
#include "Network.h"
#include "unity.h"

void setUp(void) {
    ESP_ReturnTrue();
    Network_init();
}

void tearDown(void) {
    TEST_ASSERT_EQUAL(NOT_CONNECTED, NetworkStatus.MQTTStatus);
}

void TEST_NETWORK_INIT(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, NetworkStatus.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, NetworkStatus.WIFIStatus);
}

void TEST_CONNECT_TO_NETWORK(void) {
    Network_ConnectToNetworkPlain("SSID", "password");
    TEST_ASSERT_EQUAL(CONNECTED, NetworkStatus.WIFIStatus);
    Network_DisconnectFromNetwork();
}

void TEST_CONNECT_TO_NETWORK_TWICE(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, NetworkStatus.ChipStatus);
    Network_ConnectToNetworkPlain("SSID", "password");
    TEST_ASSERT_EQUAL(CONNECTED, NetworkStatus.WIFIStatus);
    Network_ConnectToNetworkPlain("SSID2", "password2");
    TEST_ASSERT_EQUAL(CONNECTED, NetworkStatus.WIFIStatus);
    Network_DisconnectFromNetwork();
}

void TEST_CONNECT_TO_NETWORK_FAIL(void) {
    ESP_ReturnFalse();
    Network_ConnectToNetworkPlain("SSID", "password");
    TEST_ASSERT_EQUAL(NOT_CONNECTED, NetworkStatus.WIFIStatus);
    Network_DisconnectFromNetwork();
}

void TEST_DISCONNECT_FROM_NETWORK(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, NetworkStatus.ChipStatus);
    Network_ConnectToNetworkPlain("SSID", "password");
    TEST_ASSERT_EQUAL(CONNECTED, NetworkStatus.WIFIStatus);
    Network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(NOT_CONNECTED, NetworkStatus.WIFIStatus);
}

void TEST_DISCONNECT_FROM_NETWORK_TWICE(void) {
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, NetworkStatus.ChipStatus);
    Network_ConnectToNetworkPlain("SSID", "password");
    TEST_ASSERT_EQUAL(CONNECTED, NetworkStatus.WIFIStatus);
    Network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(NOT_CONNECTED, NetworkStatus.WIFIStatus);
    Network_DisconnectFromNetwork();
    TEST_ASSERT_EQUAL(NOT_CONNECTED, NetworkStatus.WIFIStatus);
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
