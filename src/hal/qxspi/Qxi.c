#include "hardware/spi.h"

#include "Gpio.h"
#include "Qxi.h"
#include "Spi.h"

static spiConfiguration_t spiToFpgaConfig = {.spiInstance = QXI_SPI,
                                             .baudrate = QXI_BAUD_RATE,
                                             .misoPin = QXI_SPI_RX_PIN,
                                             .mosiPin = QXI_SPI_TX_PIN,
                                             .sckPin = QXI_SPI_SCK_PIN,
                                             .csPin = QXI_SPI_CS_PIN};

void qxiInit(void) {
    // The Configuration is defined in the header file.
    spi_init(QXI_SPI, QXI_BAUD_RATE);
    spi_set_format(QXI_SPI, 8, QXI_CPOL, QXI_CPHA, QXI_ORDER);

    gpioSetPinFunction(QXI_SPI_RX_PIN, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(QXI_SPI_TX_PIN, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(QXI_SPI_SCK_PIN, GPIO_FUNCTION_SPI);

    gpioInitPin(QXI_SPI_CS_PIN, GPIO_OUTPUT);
}

void qxiDeinit(void) {
    spi_deinit(QXI_SPI);
    spi_set_format(QXI_SPI, 8, QXI_CPOL, QXI_CPHA, QXI_ORDER);

    gpioSetPinFunction(QXI_SPI_RX_PIN, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(QXI_SPI_TX_PIN, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(QXI_SPI_SCK_PIN, GPIO_FUNCTION_NULL);

    gpioDisablePin(QXI_SPI_RX_PIN);
    gpioDisablePin(QXI_SPI_TX_PIN);
    gpioDisablePin(QXI_SPI_SCK_PIN);

    gpioDisablePin(QXI_SPI_CS_PIN);
}

void qxiReadBlocking(uint16_t startAddress, uint8_t *dataBuffer, size_t length) {
    uint8_t cmd[3] = {QXI_READ_COMMAND, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = 3};

    data_t dataToRead = {.data = dataBuffer, .length = length};

    spiWriteCommandAndReadBlocking(&spiToFpgaConfig, &command, &dataToRead);
}

void qxiWriteBlocking(uint16_t startAddress, uint8_t *data, size_t length) {
    uint8_t cmd[3] = {QXI_WRITE_COMMAND, startAddress >> 8, startAddress};
    data_t command = {.data = cmd, .length = 3};

    data_t dataToWrite = {.data = data, .length = length};

    spiWriteCommandAndDataBlocking(&spiToFpgaConfig, &command, &dataToWrite);
}
