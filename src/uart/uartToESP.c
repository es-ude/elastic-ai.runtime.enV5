#include "uartToESP.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "string.h"

UARTDevice uartDev;

// RX interrupt handler
void callback_uart_rx_interrupt() {
    while (uart_is_readable((uart_inst_t*)uartDev.uartInstance)) {
        char ch = uart_getc((uart_inst_t*)uartDev.uartInstance);
        uartDev.receive_buf[uartDev.receive_count] = ch;
        uartDev.receive_count++;
        uartDev.receive_buf[uartDev.receive_count] = 0;
    }
}

void uartToEsp_Init(UARTDevice device) {

    if(device.uartId==0)
        device.uartInstance = (UartInstance*)uart0;
    else if(device.uartId == 1)
        device.uartInstance = (UartInstance*)uart1;
    else
        return;

    uartDev = device;
    // Set up our UART with a basic baud rate.
    uart_init((uart_inst_t*)uartDev.uartInstance, uartDev.baud_rate_set);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(uartDev.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(uartDev.rx_pin, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    uartDev.baud_rate_actual = uart_set_baudrate((uart_inst_t*)uartDev.uartInstance, uartDev.baud_rate_set);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow((uart_inst_t*)uartDev.uartInstance, false, false);

    // Set our data format
    uart_set_format((uart_inst_t*)uartDev.uartInstance, uartDev.data_bits, uartDev.stop_bits,
                    (uart_parity_t)uartDev.parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled((uart_inst_t*)uartDev.uartInstance, false);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART1_IRQ, callback_uart_rx_interrupt);
    irq_set_enabled(UART1_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables((uart_inst_t*)uartDev.uartInstance, true, false);
}

void uartToESP_Println(char *data) {
    uart_puts((uart_inst_t*)uartDev.uartInstance, data);
    uart_puts((uart_inst_t*)uartDev.uartInstance, "\r\n");
}

void uartToESP_CleanReceiveBuffer(void) {
    uartDev.receive_buf[0] = 0;
    uartDev.receive_count  = 0;
}

bool uartToESP_ResponseArrived(char *expected_response) {
    return (strstr(uartDev.receive_buf, expected_response) != NULL);
}

bool uartToESP_Readln(char *startAtString, char *responseBuf) {
    if (uartDev.receive_count > 0) {
        char *start = strstr(uartDev.receive_buf, startAtString);
        if (start != NULL) {
            char *end = strstr(start, "\r\n");
            if (end != NULL) {
                memcpy(responseBuf, start + strlen(startAtString), end - start);
                responseBuf[end - start + 1] = 0;
                return true;
            }
        }
    }
    return false;
}

bool uartToESP_Read(char *startAtString, char *responseBuf, int length) {
    if (uartDev.receive_count > 0) {
        char *start = strstr(uartDev.receive_buf, startAtString);
        if (start != NULL) {
            memcpy(responseBuf, start + strlen(startAtString), length - 1);
            responseBuf[length] = 0;
            return true;
        }
    }
    return false;
}

bool uartToESP_Cut(char *startAtString, char *responseBuf, int length) {
    if (uartDev.receive_count > 0) {
        char *start = strstr(uartDev.receive_buf, startAtString);
        if (start != NULL) {
            memcpy(responseBuf, start + strlen(startAtString), length);
            responseBuf[length] = 0;
            uartDev.receive_count -= strlen(startAtString) + length;
            strcpy(start, start + strlen(startAtString) + length);
            return true;
        }
    }
    return false;
}

// DO NOT USE except for debugging purposes
char *uartToESP_GetReceiveBuffer() { return uartDev.receive_buf; }