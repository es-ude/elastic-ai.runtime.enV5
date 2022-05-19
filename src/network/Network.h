#ifndef NETWORK_H
#define NETWORK_H

#include "uartToESP.h"
#include <stdbool.h>
#include <stdint.h>

#define ESP_CHIP_OK 1
#define ESP_CHIP_NOT_OK 0
#define CONNECTED 1
#define NOT_CONNECTED 0

typedef struct {
    // Can the two chips communicate?
    int ChipStatus;
    // Is the esp connected to a Wi-Fi
    int WIFIStatus;
    // is the esp connected to a server?
    int TCPStatus;
    // is the board connected to an MQTT Broker
    int MQTTStatus;
} NetworkStatus_t;

typedef struct {
    // the name of your WI-FI
    char *ssid;
    // the Password for your WI-FI
    char *password;
} NetworkCredentials;

static UARTDevice esp32UartDevice = {
        // Below depends on the hardware connection
        // you should modify it according to your hardware
        .name     = "uart_to_esp32",
        .uartId  = 1,
        .tx_pin   = 4,
        .rx_pin   = 5,

        // Below depends on the firmware on the esp32 module
        .baudrate_set    = 115200,
        .baudrate_actual = 0,
        .data_bits        = 8,
        .stop_bits        = 1,
        .parity           = UartParity_NONE,
};

extern NetworkStatus_t NetworkStatus;

bool Network_init(void);

bool Network_ConnectToNetwork(NetworkCredentials credentials);

void Network_ConnectToNetworkPlain(char *ssid, char *password);

void Network_DisconnectFromNetwork(void);

void Network_PrintIP(void);

void Network_Ping(char *ipAddress);

#endif
