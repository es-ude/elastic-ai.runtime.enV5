#ifndef SENSOR_BOARD_ESPBASE_H
#define SENSOR_BOARD_ESPBASE_H

#include "stdbool.h"
#include "stdint.h"
#include "uart/uartToESP.h"

#define ESP_CHIP_OK 1
#define ESP_CHIP_NOT_OK 1
#define CONNECTED 1
#define NOT_CONNECTED 0

// Speed at which the uart response buffer is checked in MS
#define REFRESH_RESPOND_IN_MS 500

typedef struct {
    // Can the two chips communicate?
    int ChipStatus;
    // Is the esp connected to a Wi-Fi
    int WIFIStatus;
    // is the esp connected to a server?
    int TCPStatus;
    // is the board connected to an MQTT Broker
    int MQTTStatus;

} ESP_StatusFlags;
extern ESP_StatusFlags ESPChipStatusFlags;

ESP_StatusFlags ESP_GetStatusFlags();

static UARTDevice esp32UartDevice = {
        // Below depends on the hardware connection
        // you should modify it according to your hardware
        .name     = "uart_to_esp32",
        .uartId  = 1,
        .tx_pin   = 4,
        .rx_pin   = 5,

        // Below depends on the firmware on the esp32 module
        .baud_rate_set    = 115200,
        .baud_rate_actual = 0,
        .data_bits        = 8,
        .stop_bits        = 1,
        .parity           = UartParity_NONE,
        .receive_count    = 0,
};

bool ESP_Init(bool softInit);

void ESP_GetIP(void);

void ESP_SoftReset();

typedef struct {
    // the name of your WI-FI
    char *ssid;
    // the Password for your WI-FI
    char *password;
} ESP_NetworkCredentials;

bool ESP_ConnectToNetwork(ESP_NetworkCredentials credentials);

bool ESP_DisconnectFromNetwork();

bool MQTT_connected(void);

bool ESP_Ping(char *ipAddress);

void ESP_CleanReceiveBuffer();

static bool CheckIsESPResponding();

static void DisableEcho();
// Do not use! if you don't know what you are doing
bool ESP_SendCommand(char *cmd, char *expectedResponse, int timeoutMs);

#endif // SENSOR_BOARD_ESPBASE_H
