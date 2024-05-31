#ifndef ENV5_BUSCONFIG_HEADER
#define ENV5_BUSCONFIG_HEADER

#include "I2cTypedefs.h"
#include "Uart.h"

#include "hardware/i2c.h"
#include "hardware/uart.h"

#define I2C_INSTANCE i2c1
#define I2C_FREQUENCY_IN_HZ 400000
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
extern i2cConfiguration_t i2CConfiguration;

#define UART_INSTANCE uart1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define UART_BAUDRATE 115200
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY NoneParity
extern uartConfiguration_t uartConfiguration;

#endif // ENV5_BUSCONFIG_HEADER
