#ifndef ENV5_HW_CONFIGURATION_REV_ONE_HEADER
#define ENV5_HW_CONFIGURATION_REV_ONE_HEADER

#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/spi.h"

/* region I2C */
#define I2C_INSTANCE i2c1
#define I2C_FREQUENCY_IN_HZ 400000
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
/* endregion I2C */

/* region UART */
#define UART_INSTANCE uart1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define UART_BAUDRATE 115200
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY NoneParity
/* endregion UART */

/* region FLASH*/
//! depends on part number
#define FLASH_BYTES_PER_PAGE 512

//! Each sector consists of 256kB (= 262144B)
#define FLASH_BYTES_PER_SECTOR 262144

#define SPI_FLASH_INSTANCE spi0
#define SPI_FLASH_SCK 2
#define SPI_FLASH_MISO 0
#define SPI_FLASH_MOSI 3
#define SPI_FLASH_BAUDRATE 1000000
#define SPI_FLASH_CS 1
/* endregion FLASH*/

/* region FPGA */
#define SPI_FPGA_INSTANCE spi0
#define SPI_FPGA_SCK 18
#define SPI_FPGA_MISO 16
#define SPI_FPGA_MOSI 19
#define SPI_FPGA_BAUDRATE 62000000
#define SPI_FPGA_CS 17
/* endregion FPGA */

/* region ADXL345b */
//! ADXL_SLAVE is the ADXL345B_I2C_ALTERNATE_ADDRESS which can be found in src/sensor/adxl345b/include/Adxl345bTypedefs.h
#define ADXL_SLAVE 0x53
#define ADXL_HOST I2C_INSTANCE
/* endregion ADXL345b */

/* region PAC193x I */
//! PAC_ONE_SLAVE is the PAC193X_I2C_ADDRESS_499R which can be found in src/sensor/pac193x/include/Pac193xTypedefs.h
#define PAC_ONE_SLAVE 0x11
#define PAC_ONE_HOST I2C_INSTANCE
#define PAC_ONE_POWER_PIN -1
#define PAC_ONE_R_SENSE { 0.82f, 0.82f, 0.82f, 0.82f }
#define PAC_ONE_USED_CHANNELS { .uint_channelsInUse = 0b00001111 }
/* endregion PAC193x I*/

/* region PAC193x II */
//! PAC_TWO_SLAVE is the PAC193X_I2C_ADDRESS_806R which can be found in src/sensor/pac193x/include/Pac193xTypedefs.h
#define PAC_TWO_SLAVE 0x12
#define PAC_TWO_HOST I2C_INSTANCE
#define PAC_TWO_POWER_PIN -1
#define PAC_TWO_R_SENSE { 0.82f, 0.82f, 0.82f, 0.82f }
#define PAC_TWO_USED_CHANNELS { .uint_channelsInUse = 0b00001111 }
/* endregion PAC193x II */

/* region SHT*/
//! SHT_SLAVE is the SHT3X_I2C_ADDRESS which can be found in src/sensor/sht3x/include/Sht3xTypedefs.h
#define SHT_SLAVE 0x44
#define SHT_HOST I2C_INSTANCE
/* endregion SHT */

#endif // ENV5_HW_CONFIGURATION_REV_ONE_HEADER
