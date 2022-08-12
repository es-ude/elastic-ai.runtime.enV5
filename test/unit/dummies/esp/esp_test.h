#ifndef ENV5_ESP_TEST_H
#define ENV5_ESP_TEST_H

#define ESP_CHIP_OK 1
#define ESP_CHIP_NOT_OK 0
#define CONNECTED 1
#define NOT_CONNECTED 0

typedef struct {
    // Can the two chips communicate?
    int ChipStatus;
    // Is the esp connected to a Wi-Fi
    int WIFIStatus;
    // is the board connected to an MQTT Broker
    int MQTTStatus;
} ESP_Status_t;

extern ESP_Status_t ESP_Status;

void esp_ReturnTrue(void);

void esp_ReturnFalse(void);

#endif /* ENV5_ESP_TEST_H */
