#ifndef ENV5_HW_CONFIGURATION
#define ENV5_HW_CONFIGURATION

/* region SPI */

#ifndef SPI_DEFAULT_BAUDRATE
#define SPI_DEFAULT_BAUDRATE 1000000
#endif

/* region SPI 0 */

#ifndef SPI0_MODULE
#define SPI0_MODULE SPI_INSTANCE(0)
#endif

#ifndef SPI0_MISO
#define SPI0_MISO 0
#endif

#ifndef SPI0_MOSI
#define SPI0_MOSI 3
#endif

#ifndef SPI0_CLOCK
#define SPI0_CLOCK 2
#endif

/* endregion SPI 0 */

/* region SPI 1 */

#ifndef SPI1_MODULE
#define SPI1_MODULE SPI_INSTANCE(1)
#endif

#ifndef SPI1_MISO
#define SPI1_MISO 8
#endif

#ifndef SPI1_MOSI
#define SPI1_MOSI 11
#endif

#ifndef SPI1_CLOCK
#define SPI1_CLOCK 10
#endif

/* endregion SPI 1 */

/* endregion SPI */

/* region I2C */
#ifndef I2C_MODULE
#define I2C_MODULE I2C_INSTANCE(1)
#endif

#ifndef I2C_FREQUENCY_IN_HZ
#define I2C_FREQUENCY_IN_HZ 400000
#endif

#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN 6
#endif

#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN 7
#endif

/* endregion I2C */

/* region UART */
#ifndef UART_MODULE
#define UART_MODULE UART_INSTANCE(1)
#endif

#ifndef UART_TX_PIN
#define UART_TX_PIN 4
#endif

#ifndef UART_RX_PIN
#define UART_RX_PIN 5
#endif

#ifndef UART_BAUDRATE
#define UART_BAUDRATE 115200
#endif

#ifndef UART_DATA_BITS
#define UART_DATA_BITS 8
#endif

#ifndef UART_STOP_BITS
#define UART_STOP_BITS 1
#endif

#ifndef UART_PARITY
#define UART_PARITY NoneParity
#endif

/* endregion UART */

/* region FLASH*/
#ifndef FLASH_BYTES_PER_PAGE
//! depends on part number
#define FLASH_BYTES_PER_PAGE 512
#endif

#ifndef FLASH_BYTES_PER_SECTOR
//! Each sector consists of 256kB (= 262144B)
#define FLASH_BYTES_PER_SECTOR 262144
#endif

#ifndef FLASH_SPI_MODULE
#define FLASH_SPI_MODULE SPI_INSTANCE(0)
#endif

#ifndef FLASH_SPI_CLOCK
#define FLASH_SPI_CLOCK 2
#endif

#ifndef FLASH_SPI_MISO
#define FLASH_SPI_MISO 0
#endif

#ifndef FLASH_SPI_MOSI
#define FLASH_SPI_MOSI 3
#endif

#ifndef FLASH_SPI_BAUDRATE
#define FLASH_SPI_BAUDRATE 1000000
#endif

#ifndef FLASH_SPI_CS
#define FLASH_SPI_CS 1
#endif

/* endregion FLASH*/

/* region FPGA */
/* region SPI CONFIG */
#ifndef FPGA_SPI_MODULE
#define FPGA_SPI_MODULE SPI0_MODULE
#endif

#ifndef FPGA_SPI_CLOCK
#define FPGA_SPI_CLOCK 18
#endif

#ifndef FPGA_SPI_MISO
#define FPGA_SPI_MISO 16
#endif

#ifndef FPGA_SPI_MOSI
#define FPGA_SPI_MOSI 19
#endif

#ifndef FPGA_SPI_BAUDRATE
#define FPGA_SPI_BAUDRATE 62000000
#endif

#ifndef FPGA_SPI_CS
#define FPGA_SPI_CS 17
#endif
/* endregion SPI CONFIG */

#ifndef FPGA_VOL_REGULATOR_EN_PIN
//! HIGH -> on, LOW -> off
#define FPGA_VOL_REGULATOR_EN_PIN 23
#endif

#ifndef FPGA_MOS_EN_PIN
//! LOW -> on, HIGH -> off
#define FPGA_MOS_EN_PIN 21
#endif

#ifndef FPGA_RESET_CTRL_PIN
//! LOW -> on, HIGH -> off
#define FPGA_RESET_CTRL_PIN 12
#endif

#ifndef FPGA_BUSY_PIN
#define FPGA_BUSY_PIN 15
#endif
/* endregion FPGA */

/* region ADXL345b */
#ifndef ADXL_SLAVE
//! ADXL_SLAVE is the ADXL345B_I2C_ALTERNATE_ADDRESS which can be found in
//! src/sensor/adxl345b/include/Adxl345bTypedefs.h
#define ADXL_SLAVE 0x53
#endif

#ifndef ADXL_I2C_MODULE
#define ADXL_I2C_MODULE I2C_MODULE
#endif

/* endregion ADXL345b */

/* region PAC193x I */
#ifndef PAC_ONE_SLAVE
//! PAC_ONE_SLAVE is the PAC193X_I2C_ADDRESS_499R which can be found in
//! src/sensor/pac193x/include/Pac193xTypedefs.h
#define PAC_ONE_SLAVE 0x11
#endif

#ifndef PAC_ONE_I2C_MODULE
#define PAC_ONE_I2C_MODULE I2C_MODULE
#endif

#ifndef PAC_ONE_POWER_PIN
#define PAC_ONE_POWER_PIN -1
#endif

#ifndef PAC_ONE_R_SENSE
#define PAC_ONE_R_SENSE                                                                            \
    { 0.82f, 0.82f, 0.82f, 0.82f }
#endif

#ifndef PAC_ONE_USED_CHANNELS
#define PAC_ONE_USED_CHANNELS                                                                      \
    { .uint_channelsInUse = 0b00001111 }
#endif

/* endregion PAC193x I*/

/* region PAC193x II */
#ifndef PAC_TWO_SLAVE
//! PAC_TWO_SLAVE is the PAC193X_I2C_ADDRESS_806R which can be found in
//! src/sensor/pac193x/include/Pac193xTypedefs.h
#define PAC_TWO_SLAVE 0x12
#endif

#ifndef PAC_TWO_I2C_MODULE
#define PAC_TWO_I2C_MODULE I2C_MODULE
#endif

#ifndef PAC_TWO_POWER_PIN
#define PAC_TWO_POWER_PIN -1
#endif

#ifndef PAC_TWO_R_SENSE
#define PAC_TWO_R_SENSE                                                                            \
    { 0.82f, 0.82f, 0.82f, 0.82f }
#endif

#ifndef PAC_TWO_USED_CHANNELS
#define PAC_TWO_USED_CHANNELS                                                                      \
    { .uint_channelsInUse = 0b00001111 }
#endif

/* endregion PAC193x II */

/* region SHT*/
#ifndef SHT_SLAVE
//! SHT_SLAVE is the SHT3X_I2C_ADDRESS which can be found in
//! src/sensor/sht3x/include/Sht3xTypedefs.h
#define SHT_SLAVE 0x44
#endif

#ifndef SHT_I2C_MODULE
#define SHT_I2C_MODULE I2C_MODULE
#endif

/* endregion SHT */

/* region HTTP BIN-FILE DOWNLOAD */
#ifndef HTTP_CHUNK_SIZE
#define HTTP_CHUNK_SIZE FLASH_BYTES_PER_PAGE
#endif
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

/* region BMI323 */

#ifndef BMI323_SPI_MODULE
#define BMI323_SPI_MODULE SPI1_MODULE
#endif

#ifndef BMI323_SPI_BAUDRATE
#define BMI323_SPI_BAUDRATE SPI_DEFAULT_BAUDRATE
#endif

#ifndef BMI323_SPI_MISO
#define BMI323_SPI_MISO SPI1_MISO
#endif

#ifndef BMI323_SPI_MOSI
#define BMI323_SPI_MOSI SPI1_MOSI
#endif

#ifndef BMI323_SPI_CLOCK
#define BMI323_SPI_CLOCK SPI1_CLOCK
#endif

#ifndef BMI323_SPI_CHIP_SELECT
#define BMI323_SPI_CHIP_SELECT 9
#endif

/* endregion BMI323 */

#endif
