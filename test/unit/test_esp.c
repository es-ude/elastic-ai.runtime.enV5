#include "esp/esp.h"
#include "unity.h"

volatile ESP_Status_t ESP_Status;

void setUp(void) {
    //Generate ESP_Status after esp_Init(void)
    ESP_Status.ChipStatus = ESP_CHIP_OK;
    ESP_Status.WIFIStatus = NOT_CONNECTED;
    ESP_Status.MQTTStatus = NOT_CONNECTED;
}

void tearDown(void) {}

/* region Tests */

void test_esp_CheckIsResponding_failed(void) {
    // TODO
}
void test_esp_CheckIsResponding_successful(void) {
    // TODO
}

void test_esp_SoftReset_failed(void) {
    // TODO
}
void test_esp_SoftReset_successful(void) {
    // TODO
}

void test_esp_SetMQTTReceiverFunction_successful(void) {
    // TODO
}

void test_esp_SendCommand_failed_uart_busy(void) {
    // TODO
}
void test_esp_SendCommand_successful_send_command(void) {
    // TODO
}
void test_esp_SendCommand_failed_response_missing(void) {
    // TODO
}
void test_esp_SendCommand_successful_received_response(void) {
    // TODO
}

/* endregion */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_esp_CheckIsResponding_failed);
    RUN_TEST(test_esp_CheckIsResponding_successful);

    RUN_TEST(test_esp_SoftReset_failed);
    RUN_TEST(test_esp_SoftReset_successful);

    RUN_TEST(test_esp_SetMQTTReceiverFunction_successful);

    RUN_TEST(test_esp_SendCommand_failed_uart_busy);
    RUN_TEST(test_esp_SendCommand_successful_send_command);
    RUN_TEST(test_esp_SendCommand_failed_response_missing);
    RUN_TEST(test_esp_SendCommand_successful_received_response);

    return UNITY_END();
}