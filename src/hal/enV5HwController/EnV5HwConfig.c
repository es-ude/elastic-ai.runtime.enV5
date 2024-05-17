#include "BusConfig.h"
#include "SensorConfig.h"

spiConfiguration_t fpgaSpiConfiguration = {.spiInstance = SPI_FPGA_INSTANCE,
                                           .sckPin = SPI_FPGA_SCK,
                                           .misoPin = SPI_FPGA_MISO,
                                           .mosiPin = SPI_FPGA_MOSI,
                                           .baudrate = SPI_FPGA_BAUDRATE,
                                           .csPin = SPI_FPGA_CS};

spiConfiguration_t flashSpiConfiguration = {.spiInstance = SPI_FLASH_INSTANCE,
                                            .misoPin = SPI_FLASH_MISO,
                                            .sckPin = SPI_FLASH_SCK,
                                            .mosiPin = SPI_FLASH_MOSI,
                                            .baudrate = SPI_FLASH_BAUDRATE,
                                            .csPin = SPI_FLASH_CS

};

i2cConfiguration_t i2cConfiguration = {.i2cInstance = I2C_INSTANCE,
                                       .sdaPin = I2C_SDA_PIN,
                                       .sclPin = I2C_SCL_PIN,
                                       .frequency = I2C_FREQUENCY_IN_HZ};

uartConfiguration_t uartConfiguration = {.uartInstance = UART_INSTANCE,
                                         .txPin = UART_TX_PIN,
                                         .rxPin = UART_RX_PIN,
                                         .baudrate = UART_BAUDRATE,
                                         .dataBits = UART_DATA_BITS,
                                         .stopBits = UART_STOP_BITS,
                                         .parity = UART_PARITY};

adxl345bSensorConfiguration_t adxl345bI2cSensorConfiguration = {
    .i2c_slave_address = ADXL_SLAVE,
    .i2c_host = ADXL_HOST,
};

pac193xSensorConfiguration_t pac193xI2cSensorConfigurationOne = {
    .i2c_slave_address = PAC_ONE_SLAVE,
    .i2c_host = PAC_ONE_HOST,
    .powerPin = PAC_ONE_POWER_PIN,
    .rSense = PAC_ONE_R_SENSE,
    .usedChannels = PAC_ONE_USED_CHANNELS,
};

pac193xSensorConfiguration_t pac193xI2cSensorConfigurationTwo = {
    .i2c_slave_address = PAC_TWO_SLAVE,
    .i2c_host = PAC_TWO_HOST,
    .powerPin = PAC_TWO_POWER_PIN,
    .rSense = PAC_TWO_R_SENSE,
    .usedChannels = PAC_TWO_USED_CHANNELS,
};

sht3xSensorConfiguration_t sht3xI2cSensorConfiguration = {
    .i2c_slave_address = SHT_SLAVE,
    .i2c_host = SHT_HOST,
};
