#ifndef ENV5_HW_CONFIGURATION
#define ENV5_HW_CONFIGURATION

/* region I2C */
#ifndef I2C_INSTANCE
#define I2C_INSTANCE i2c1
#endif // I2C_INSTANCE

#ifndef I2C_FREQUENCY_IN_HZ
#define I2C_FREQUENCY_IN_HZ 400000
#endif // I2C_FREQUENCY_IN_HZ

#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN 6
#endif // I2C_SDA_PIN

#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN 7
#endif // I2C_SCL_PIN

/* endregion I2C */

/* region UART */
#ifndef UART_INSTANCE
#define UART_INSTANCE uart1
#endif // UART_INSTANCE

#ifndef UART_TX_PIN
#define UART_TX_PIN 4
#endif // UART_TX_PIN

#ifndef UART_RX_PIN
#define UART_RX_PIN 5
#endif // UART_RX_PIN

#ifndef UART_BAUDRATE
#define UART_BAUDRATE 115200
#endif // UART_BAUDRATE

#ifndef UART_DATA_BITS
#define UART_DATA_BITS 8
#endif // UART_DATA_BITS

#ifndef UART_STOP_BITS
#define UART_STOP_BITS 1
#endif // UART_STOP_BITS

#ifndef UART_PARITY
#define UART_PARITY NoneParity
#endif // UART_PARITY

/* endregion UART */

/* region FLASH*/
#ifndef FLASH_BYTES_PER_PAGE
//! depends on part number
#define FLASH_BYTES_PER_PAGE 512
#endif // FLASH_BYTES_PER_PAGE

#ifndef FLASH_BYTES_PER_SECTOR
//! Each sector consists of 256kB (= 262144B)
#define FLASH_BYTES_PER_SECTOR 262144
#endif // FLASH_BYTES_PER_SECTOR

#ifndef FLASH_SPI_INSTANCE
#define FLASH_SPI_INSTANCE spi0
#endif // SPI_FLASH_INSTANCE

#ifndef FLASH_SPI_CLOCK
#define FLASH_SPI_CLOCK 2
#endif // SPI_FLASH_SCK

#ifndef FLASH_SPI_MISO
#define FLASH_SPI_MISO 0
#endif // SPI_FLASH_MISO

#ifndef FLASH_SPI_MOSI
#define FLASH_SPI_MOSI 3
#endif // SPI_FLASH_MOSI

#ifndef FLASH_SPI_BAUDRATE
#define FLASH_SPI_BAUDRATE 1000000
#endif // SPI_FLASH_BAUDRATE

#ifndef FLASH_SPI_CS
#define FLASH_SPI_CS 1
#endif // SPI_FLASH_CS

/* endregion FLASH*/

/* region FPGA */
/* region SPI CONFIG */
#ifndef FPGA_SPI_INSTANCE
#define FPGA_SPI_INSTANCE spi0
#endif // SPI_FPGA_INSTANCE

#ifndef FPGA_SPI_CLOCK
#define FPGA_SPI_CLOCK 18
#endif // SPI_FPGA_SCK

#ifndef FPGA_SPI_MISO
#define FPGA_SPI_MISO 16
#endif // SPI_FPGA_MISO

#ifndef FPGA_SPI_MOSI
#define FPGA_SPI_MOSI 19
#endif // SPI_FPGA_MOSI

#ifndef FPGA_SPI_BAUDRATE
#define FPGA_SPI_BAUDRATE 62000000
#endif // SPI_FPGA_BAUDRATE

#ifndef FPGA_SPI_CS
#define FPGA_SPI_CS 17
#endif // SPI_FPGA_CS
/* endregion SPI CONFIG */

#ifndef FPGA_VOL_REGULATOR_EN_PIN
//! HIGH -> on, LOW -> off
#define FPGA_VOL_REGULATOR_EN_PIN 23
#endif // FPGA_VOL_REGULATOR_EN_PIN

#ifndef FPGA_MOS_EN_PIN
//! LOW -> on, HIGH -> off
#define FPGA_MOS_EN_PIN 21
#endif // FPGA_MOS_EN_PIN

#ifndef FPGA_RESET_CTRL_PIN
//! LOW -> on, HIGH -> off
#define FPGA_RESET_CTRL_PIN 12
#endif // FPGA_RESET_CTRL_PIN#define FPGA_BUSY_PIN 15

#ifndef FPGA_BUSY_PIN
#define FPGA_BUSY_PIN 15
#endif // FPGA_BUSY_PIN
/* endregion FPGA */

/* region ADXL345b */
#ifndef ADXL_SLAVE
//! ADXL_SLAVE is the ADXL345B_I2C_ALTERNATE_ADDRESS which can be found in
//! src/sensor/adxl345b/include/Adxl345bTypedefs.h
#define ADXL_SLAVE 0x53
#endif // ADXL_SLAVE

#ifndef ADXL_HOST
#define ADXL_HOST I2C_INSTANCE
#endif // ADXL_HOST

/* endregion ADXL345b */

/* region PAC193x I */
#ifndef PAC_ONE_SLAVE
//! PAC_ONE_SLAVE is the PAC193X_I2C_ADDRESS_499R which can be found in
//! src/sensor/pac193x/include/Pac193xTypedefs.h
#define PAC_ONE_SLAVE 0x11
#endif // PAC_ONE_SLAVE

#ifndef PAC_ONE_HOST
#define PAC_ONE_HOST I2C_INSTANCE
#endif // PAC_ONE_HOST

#ifndef PAC_ONE_POWER_PIN
#define PAC_ONE_POWER_PIN -1
#endif // PAC_ONE_POWER_PIN

#ifndef PAC_ONE_R_SENSE
#define PAC_ONE_R_SENSE                                                                            \
    { 0.82f, 0.82f, 0.82f, 0.82f }
#endif // PAC_ONE_R_SENSE

#ifndef PAC_ONE_USED_CHANNELS
#define PAC_ONE_USED_CHANNELS                                                                      \
    { .uint_channelsInUse = 0b00001111 }
#endif // PAC_ONE_USED_CHANNELS

/* endregion PAC193x I*/

/* region PAC193x II */
#ifndef PAC_TWO_SLAVE
//! PAC_TWO_SLAVE is the PAC193X_I2C_ADDRESS_806R which can be found in
//! src/sensor/pac193x/include/Pac193xTypedefs.h
#define PAC_TWO_SLAVE 0x12
#endif // PAC_TWO_SLAVE

#ifndef PAC_TWO_HOST
#define PAC_TWO_HOST I2C_INSTANCE
#endif // PAC_TWO_HOST

#ifndef PAC_TWO_POWER_PIN
#define PAC_TWO_POWER_PIN -1
#endif // PAC_TWO_POWER_PIN

#ifndef PAC_TWO_R_SENSE
#define PAC_TWO_R_SENSE                                                                            \
    { 0.82f, 0.82f, 0.82f, 0.82f }
#endif // PAC_TWO_R_SENSE

#ifndef PAC_TWO_USED_CHANNELS
#define PAC_TWO_USED_CHANNELS                                                                      \
    { .uint_channelsInUse = 0b00001111 }
#endif // PAC_TWO_USED_CHANNELS

/* endregion PAC193x II */

/* region SHT*/
#ifndef SHT_SLAVE
//! SHT_SLAVE is the SHT3X_I2C_ADDRESS which can be found in
//! src/sensor/sht3x/include/Sht3xTypedefs.h
#define SHT_SLAVE 0x44
#endif // SHT_SLAVE

#ifndef SHT_HOST
#define SHT_HOST I2C_INSTANCE
#endif // SHT_HOST

/* endregion SHT */

/* region HTTP BIN-FILE DOWNLOAD */
#ifndef HTTP_CHUNK_SIZE
#define HTTP_CHUNK_SIZE FLASH_BYTES_PER_PAGE
#endif // HTTP_CHUNK_SIZE
/* endregion HTTP BIN-FILE DOWNLOAD */

/* region LED */
#ifndef LED0_GPIO
#define LED0_GPIO 22
#endif

#ifndef LED1_GPIO
#define LED1_GPIO 24
#endif

#ifndef LED2_GPIO
#define LED2_GPIO 25
#endif
/* endregion LED */

#endif // ENV5_HW_CONFIGURATION
