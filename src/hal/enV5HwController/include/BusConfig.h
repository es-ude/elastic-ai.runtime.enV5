//
// Created by jasmin on 4/5/24.
//

#ifndef ENV5_BUSCONFIG_H
#define ENV5_BUSCONFIG_H

#include "../../../../build/release/_deps/pico_sdk-src/src/rp2_common/hardware_i2c/include/hardware/i2c.h"
#include "../../../../build/release/_deps/pico_sdk-src/src/rp2_common/hardware_spi/include/hardware/spi.h"
#include "../../../../build/release/_deps/pico_sdk-src/src/rp2_common/hardware_uart/include/hardware/uart.h"
#include "../../i2c/include/I2cTypedefs.h"
#include "../../spi/include/SpiTypedefs.h"
#include "../../uart/include/Uart.h"

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

#endif // ENV5_BUSCONFIG_H
