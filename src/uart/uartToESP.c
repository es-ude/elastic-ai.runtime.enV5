#define SOURCE_FILE "UART-TO-ESP"

#include "uartToESP.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include <string.h>

UARTDevice uartDev;
bool lastWasR = false;
esp_command command = {.cmd="\0", .responseArrived=false};

void (*uartToESP_MQTT_Broker_Receive)(char *) = NULL;

void handleNewLine(void) {
    if (strlen(uartDev.receive_buf) != 0) {
        if (strncmp("+MQTTSUBRECV", uartDev.receive_buf, 12) == 0 && uartToESP_MQTT_Broker_Receive != NULL) {
            uartToESP_MQTT_Broker_Receive(uartDev.receive_buf);
        }
        if (strncmp(command.expectedResponse, uartDev.receive_buf, strlen(command.expectedResponse)) == 0) {
            command.responseArrived = true;
            strcpy(command.data, &uartDev.receive_buf[strlen(command.expectedResponse)]);
        }
    }
}

// RX interrupt handler
void callback_uart_rx_interrupt() {
    while (uart_is_readable((uart_inst_t *) uartDev.uartInstance)) {
        char ch = uart_getc((uart_inst_t *) uartDev.uartInstance);

        if (ch == '\n' || ch == '\r' || ch == '\0') {
            if (lastWasR && ch == '\n') {
                handleNewLine();
                uartDev.receive_count = 0;
            }
        } else {
            uartDev.receive_buf[uartDev.receive_count] = ch;
            uartDev.receive_count++;
        }
        uartDev.receive_buf[uartDev.receive_count] = '\0';

        if (ch == '>' && uartDev.receive_count == 1) {
            handleNewLine();
            uartDev.receive_count = 0;
        }

        if (ch == '\r') {
            lastWasR = true;
        } else {
            lastWasR = false;
        }
    }
}

void uartToEsp_Init(void) {
    if (device.uartId == 0)
        device.uartInstance = (UartInstance *) uart0;
    else if (device.uartId == 1)
        device.uartInstance = (UartInstance *) uart1;
    else
        return;

    uartDev = device;
    // Set up our UART with a basic baud rate.
    uart_init((uart_inst_t *) uartDev.uartInstance, uartDev.baudrate_set);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(uartDev.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(uartDev.rx_pin, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    uartDev.baudrate_actual = uart_set_baudrate((uart_inst_t *) uartDev.uartInstance, uartDev.baudrate_set);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow((uart_inst_t *) uartDev.uartInstance, false, false);

    // Set our data format
    uart_set_format((uart_inst_t *) uartDev.uartInstance, uartDev.data_bits, uartDev.stop_bits,
                    (uart_parity_t) uartDev.parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled((uart_inst_t *) uartDev.uartInstance, false);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART1_IRQ, callback_uart_rx_interrupt);
    irq_set_enabled(UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables((uart_inst_t *) uartDev.uartInstance, true, false);

    uartDev.receive_count = 0;
    uartDev.receive_buf[0] = '\0';
}

void uartToESP_SendCommand(char *cmd, char *expectedResponse) {
    strcpy(command.cmd, cmd);
    command.responseArrived = false;
    strcpy(command.expectedResponse, expectedResponse);
    uartToESP_Println(command.cmd);
}

void uartToESP_Println(char *data) {
    uart_puts((uart_inst_t *) uartDev.uartInstance, data);
    uart_puts((uart_inst_t *) uartDev.uartInstance, "\r\n");
}

void uartToESP_SetMQTTReceiverFunction(void (*receive)(char *)) {
    uartToESP_MQTT_Broker_Receive = receive;
}

bool uartToESP_IsBusy(void) {
    if (strcmp(command.cmd, "\0") == 0)
        return false;
    return true;
}

bool uartToESP_ResponseArrived(void) {
    return command.responseArrived;
}

void uartToESP_Free(void) {
    strcpy(command.cmd, "\0");
}
