#include "CException.h"
#include "unity.h"

#include "EspUnitTest.h"
#include "eai/network/Esp.h"
#include "eai/network/MqttBroker.h"

extern mqttBrokerHost_t mqttHost;

void setUp(void) {
    espStatus.ChipStatus = ESP_CHIP_OK;
    espStatus.WIFIStatus = CONNECTED;
    espStatus.MQTTStatus = NOT_CONNECTED;

    ESPDUMMY_RETURN_CODE = ESP_NO_ERROR;
}

void tearDown(void) {}

/* region Test Functions */

void test_ConnectToMqttBrokerSuccessful(void) {
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
    CEXCEPTION_T exception_mqttBrokerConnectToBroker;
    Try {
        mqttBrokerConnectToBroker("testBroker", "testClient");
        TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
        TEST_PASS();
    }
    Catch(exception_mqttBrokerConnectToBroker) {
        TEST_FAIL_MESSAGE("Some Error occurred which caused a CException");
    }
    TEST_FAIL_MESSAGE("Something went wrong");
}

void test_ConnectToMqttBrokerEspFailed(void) {
    espStatus.ChipStatus = ESP_CHIP_NOT_OK;
    TEST_ASSERT_EQUAL(ESP_CHIP_NOT_OK, espStatus.ChipStatus);
    CEXCEPTION_T exception_mqttBrokerConnectToBroker;
    Try {
        mqttBrokerConnectToBroker("testBroker", "testClient");
        TEST_FAIL_MESSAGE("Should have thrown MQTT_ESP_CHIP_FAILED");
    }
    Catch(exception_mqttBrokerConnectToBroker) {
        TEST_ASSERT_EQUAL(MQTT_ESP_CHIP_FAILED, exception_mqttBrokerConnectToBroker);
    }
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}

void test_ConnectToMqttBrokerNoWifi(void) {
    espStatus.WIFIStatus = NOT_CONNECTED;
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.WIFIStatus);
    CEXCEPTION_T exception_mqttBrokerConnectToBroker;
    Try {
        mqttBrokerConnectToBroker("testBroker", "testClient");
        TEST_FAIL_MESSAGE("Should have thrown MQTT_WIFI_FAILED");
    }
    Catch(exception_mqttBrokerConnectToBroker) {
        TEST_ASSERT_EQUAL(MQTT_WIFI_FAILED, exception_mqttBrokerConnectToBroker);
    }
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}

void test_ConnectToMqttBrokerAlreadyConnected(void) {
    espStatus.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
    CEXCEPTION_T exception_mqttBrokerConnectToBroker;
    Try {
        mqttBrokerConnectToBroker("testBroker", "testClient");
        TEST_FAIL_MESSAGE("Should have thrown MQTT_ALREADY_CONNECTED");
    }
    Catch(exception_mqttBrokerConnectToBroker) {
        TEST_ASSERT_EQUAL(MQTT_ALREADY_CONNECTED, exception_mqttBrokerConnectToBroker);
    }
}

void test_ConnectToMqttBrokerSendCommandFailed(void) {
    ESPDUMMY_RETURN_CODE = ESP_WRONG_ANSWER_RECEIVED;
    CEXCEPTION_T exception_mqttBrokerConnectToBroker;
    Try {
        mqttBrokerConnectToBroker("testBroker", "testClient");
        TEST_FAIL_MESSAGE("Should have thrown MQTT_ESP_WRONG_ANSWER");
    }
    Catch(exception_mqttBrokerConnectToBroker) {
        TEST_ASSERT_EQUAL(MQTT_ESP_WRONG_ANSWER, exception_mqttBrokerConnectToBroker);
    }
    TEST_ASSERT_EQUAL(NOT_CONNECTED, espStatus.MQTTStatus);
}

void test_DisconnectMqttBroker() {
    espStatus.MQTTStatus = CONNECTED;
    TEST_ASSERT_EQUAL(CONNECTED, espStatus.MQTTStatus);
    CEXCEPTION_T exception_mqttBrokerDisconnect;
    Try {
        mqttBrokerDisconnect(espStatus.MQTTStatus == CONNECTED);
    }
    Catch(exception_mqttBrokerDisconnect) {
        TEST_FAIL_MESSAGE("Test did not work");
    }
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ConnectToMqttBrokerSuccessful);
    RUN_TEST(test_ConnectToMqttBrokerEspFailed);
    RUN_TEST(test_ConnectToMqttBrokerNoWifi);
    RUN_TEST(test_ConnectToMqttBrokerAlreadyConnected);
    RUN_TEST(test_ConnectToMqttBrokerSendCommandFailed);
    RUN_TEST(test_DisconnectMqttBroker);

    return UNITY_END();
}
