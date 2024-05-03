//
// Created by jasmin on 4/5/24.
//

#ifndef ENV5_BUS_H
#define ENV5_BUS_H

#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "SpiTypedefs.h"
#include "Uart.h"
#include "I2cTypedefs.h"

#define GPIO_LED0 22
#define GPIO_LED1 24
#define GPIO_LED2 25

#define FPGA_VOL_REGULATOR_EN_PIN 23 //! HIGH -> on, LOW -> off
#define FPGA_MOS_EN_PIN 21           //! LOW -> on, HIGH -> off
#define FPGA_RESET_CTRL_PIN 12       //! LOW -> on, HIGH -> off
#define FPGA_BUSY_PIN 15

#define SPI_FPGA_INSTANCE spi0
#define SPI_FPGA_SCK 18
#define SPI_FPGA_MISO 16
#define SPI_FPGA_MOSI 19
#define SPI_FPGA_BAUDRATE 62000000
#define SPI_FPGA_CS 17
extern spiConfiguration_t fpgaSpiConfiguration;

#define SPI_FLASH_INSTANCE spi0
#define SPI_FLASH_SCK 2
#define SPI_FLASH_MISO 0
#define SPI_FLASH_MOSI 3
#define SPI_FLASH_BAUDRATE 1000000
#define SPI_FLASH_CS 1
extern spiConfiguration_t flashSpiConfiguration;

#define I2C_CONTROLLER i2c1
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

#endif // ENV5_BUS_H
