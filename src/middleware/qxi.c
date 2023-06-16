//
// Created by chao on 26/12/22.
//

#include "qxi.h"
#include "Gpio.h"
#include "spi/Spi.h"
#include "hardware/spi.h"

// configuration is set in the header file.
void qxi_init(void) {
    spi_init(QXI_SPI, QXI_BAUD_RATE);
    spi_set_format(QXI_SPI, 8, QXI_CPOL, QXI_CPHA, QXI_ORDER);

    gpioSetPinFunction(QXI_SPI_RX_PIN, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(QXI_SPI_TX_PIN, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(QXI_SPI_SCK_PIN, GPIO_FUNCTION_SPI);

    gpioEnablePin(QXI_SPI_CS_PIN, GPIO_POWER_UP);
}

void qxi_deinit(void) {
    spi_deinit(QXI_SPI);
    spi_set_format(QXI_SPI, 8, QXI_CPOL, QXI_CPHA, QXI_ORDER);

    gpioSetPinFunction(QXI_SPI_RX_PIN, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(QXI_SPI_TX_PIN, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(QXI_SPI_SCK_PIN, GPIO_FUNCTION_SPI);

    gpioDisablePin(QXI_SPI_RX_PIN);
    gpioDisablePin(QXI_SPI_TX_PIN);
    gpioDisablePin(QXI_SPI_SCK_PIN);
    gpioDisablePin(QXI_SPI_CS_PIN);
}

void qxi_set_speed(uint baudrate) {
    spi_set_baudrate(QXI_SPI, baudrate);
}

void qxi_read_blocking(uint16_t addr, uint8_t *buf, size_t len) {

    uint8_t cmdbuf[3] = {QXI_READ_COMMAND, addr >> 8, addr};
    spiReadBlocking(QXI_SPI, QXI_SPI_CS_PIN, cmdbuf, 3, buf, len);
}

void qxi_write_blocking(uint16_t addr, uint8_t data[], uint16_t len) {

    uint8_t cmdbuf[3] = {QXI_WRITE_COMMAND, addr >> 8, addr};
    spiWriteBlocking(QXI_SPI, QXI_SPI_CS_PIN, cmdbuf, 3, data, len);
}