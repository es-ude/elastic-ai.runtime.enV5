#define SOURCE_FILE "ENV5-HWC"

#include<stdbool.h>

#include "Common.h"
#include "Gpio.h"
#include "Sleep.h"
#include "config/Bus.h"
#include "config/Sensor.h"
#include "controller/enV5HwController.h"

spiConfiguration_t fpgaSpiConfiguration = {
    .spiInstance = SPI_FPGA_INSTANCE,
    .sckPin = SPI_FPGA_SCK,
    .misoPin = SPI_FPGA_MISO,
    .mosiPin = SPI_FPGA_MOSI,
    .baudrate = SPI_FPGA_BAUDRATE,
    .csPin = SPI_FPGA_CS
};

spiConfiguration_t flashSpiConfiguration = {
    .spiInstance = SPI_FLASH_INSTANCE,
    .misoPin = SPI_FLASH_MISO,
    .sckPin = SPI_FLASH_SCK,
    .mosiPin = SPI_FLASH_MOSI,
    .baudrate = SPI_FLASH_BAUDRATE,
    .csPin = SPI_FLASH_CS
    
};

i2cConfiguration_t i2cConfiguration = {
    .i2cInstance = I2C_CONTROLLER,
    .sdaPin = I2C_SDA_PIN,
    .sclPin = I2C_SCL_PIN,
    .frequency = I2C_FREQUENCY_IN_HZ
};

uartConfiguration_t uartConfiguration = {
    .uartInstance = UART_INSTANCE,
    .txPin = UART_TX_PIN,
    .rxPin = UART_RX_PIN,
    .baudrate = UART_BAUDRATE,
    .dataBits = UART_DATA_BITS,
    .stopBits = UART_STOP_BITS,
    .parity = UART_PARITY
};

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


void env5HwInit() {
    env5HwLedsInit();
    env5HwFpgaInit();
    env5HwFpgaPowersOff();
}

/* region LED */

void env5HwLedsInit(void) {
    gpioInitPin(GPIO_LED0, GPIO_OUTPUT);
    gpioSetPin(GPIO_LED0, GPIO_PIN_LOW);
    PRINT_DEBUG("LED0 initialized.");

    gpioInitPin(GPIO_LED1, GPIO_OUTPUT);
    gpioSetPin(GPIO_LED1, GPIO_PIN_LOW);
    PRINT_DEBUG("LED1 initialized.");

    gpioInitPin(GPIO_LED2, GPIO_OUTPUT);
    gpioSetPin(GPIO_LED2, GPIO_PIN_LOW);
    PRINT_DEBUG("LED2 initialized.");
}

void env5HwLedsAllOn(void) {
    gpioSetPin(GPIO_LED0, GPIO_PIN_HIGH);
    gpioSetPin(GPIO_LED1, GPIO_PIN_HIGH);
    gpioSetPin(GPIO_LED2, GPIO_PIN_HIGH);
    PRINT_DEBUG("LED's enabled");
}

void env5HwLedsAllOff(void) {
    gpioSetPin(GPIO_LED0, GPIO_PIN_LOW);
    gpioSetPin(GPIO_LED1, GPIO_PIN_LOW);
    gpioSetPin(GPIO_LED2, GPIO_PIN_LOW);
    PRINT_DEBUG("LED's disabled");
}

/* endregion LED */

/* region FPGA */

// PAC1934 is using pio6 and pio7 which is connected to I2C1
void env5HwFpgaInit(void) {
    // setup FPGA reset control pin
    gpioInitPin(FPGA_RESET_CTRL_PIN, GPIO_OUTPUT);
    gpioSetPin(FPGA_RESET_CTRL_PIN, GPIO_PIN_HIGH);

    // turn the voltage regulator off => turn FPGA off
    gpioInitPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_OUTPUT);
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_LOW);

    // turn the MOS-FETS off
    gpioInitPin(FPGA_MOS_EN_PIN, GPIO_OUTPUT);
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_LOW);

    // setup FPGA busy wait pin
    gpioInitPin(FPGA_BUSY_PIN, GPIO_INPUT);
}

void env5HwFpgaPowersOn(void) {
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_HIGH); // turn the voltage regulator on
    sleep_for_ms(10);                                     // wait until the FPGA is powered up
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_LOW);            // turn the MOS-FETS on

    PRINT_DEBUG("FPGA Powered On.");
}

void env5HwFpgaPowersOff(void) {
    gpioSetPin(FPGA_VOL_REGULATOR_EN_PIN, GPIO_PIN_LOW); // turn the voltage regulator off
    gpioSetPin(FPGA_MOS_EN_PIN, GPIO_PIN_HIGH);          // turn the MOS-FETS off

    PRINT_DEBUG("FPGA Powered Off.");
}

void env5HwFpgaReset(bool enable) {
    gpioSetPin(FPGA_RESET_CTRL_PIN, enable ? GPIO_PIN_LOW : GPIO_PIN_HIGH);
}

/* endregion FPGA */
