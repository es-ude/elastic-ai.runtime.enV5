#include "unity.h"

#include "EspUnitTest.h"
#include "eai/network/Esp.h"
#include "eai/network/Network.h"

networkCredentials_t credentials = {.ssid = "SSID", .password = "password"};

void setUp(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = NOT_CONNECTED;
    espStatus.MQTTStatus = NOT_CONNECTED;

    ESPDUMMY_RETURN_CODE = ESP_NO_ERROR;
}

void tearDown(void) {}

/* region Test Functions */

void testConnectToNetworkSuccessful(void) {
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    networkErrorCode_t networkErrorCode = networkConnectToNetwork();
    TEST_ASSERT_EQUAL(NETWORK_NO_ERROR, networkErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.WIFIStatus);
}
void testConnectToNetworkAlreadyConnected(void) {
    espStatus.WIFIStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.WIFIStatus);
    networkErrorCode_t networkErrorCode = networkConnectToNetwork();
    TEST_ASSERT_EQUAL(NETWORK_WIFI_ALREADY_CONNECTED, networkErrorCode);
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.WIFIStatus);
}
void testConnectToNetworkEspChipFailed(void) {
    espStatus.ChipStatus = ESP_CHIP_NOT_OK;
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    TEST_ASSERT_EQUAL(ESP_CHIP_NOT_OK, espStatus.ChipStatus);
    networkErrorCode_t networkErrorCode = networkConnectToNetwork();
    TEST_ASSERT_EQUAL(NETWORK_ESP_CHIP_FAILED, networkErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
}
void testConnectToNetworkSendFailed(void) {
    ESPDUMMY_RETURN_CODE = ESP_WRONG_ANSWER_RECEIVED;
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    networkErrorCode_t networkErrorCode = networkConnectToNetwork();
    TEST_ASSERT_EQUAL(NETWORK_ESTABLISH_CONNECTION_FAILED, networkErrorCode);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
}

void testDisconnectFromNetwork(void) {
    espStatus.WIFIStatus = CONNECTED;
    espStatus.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.WIFIStatus);
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
    networkDisconnectFromNetwork();
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, espStatus.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}
void testDisconnectFromNetworkTwice(void) {
    espStatus.WIFIStatus = CONNECTED;
    espStatus.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.WIFIStatus);
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
    networkDisconnectFromNetwork();
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, espStatus.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
    networkDisconnectFromNetwork();
    TEST_ASSERT_EQUAL(ESP_CHIP_OK, espStatus.ChipStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testConnectToNetworkSuccessful);
    RUN_TEST(testConnectToNetworkAlreadyConnected);
    RUN_TEST(testConnectToNetworkEspChipFailed);
    RUN_TEST(testConnectToNetworkSendFailed);

    RUN_TEST(testDisconnectFromNetwork);
    RUN_TEST(testDisconnectFromNetworkTwice);

    return UNITY_END();
}
