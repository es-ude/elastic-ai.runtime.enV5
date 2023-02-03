#include "SpiHardwareAdapterInternal.h"
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <stdint.h>

void spiInitChipSelect(const uint8_t csPin) {
    gpio_init(csPin);
    gpio_set_dir(csPin, GPIO_OUT);
    gpio_put(csPin, 1);
}

void spiHardwareAdapterInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
             uint8_t misoPin) {

    spiInitChipSelect(csPin);
    spi_init(spi, baudrate);

    gpio_set_function(sckPin, GPIO_FUNC_SPI);
    gpio_set_function(mosiPin, GPIO_FUNC_SPI);
    gpio_set_function(misoPin, GPIO_FUNC_SPI);
}

void spiHardwareAdapterDeinit(spi_inst_t *spi, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
               uint8_t misoPin) {
    spi_deinit(spi);
    gpio_set_function(sckPin, GPIO_FUNC_SPI);
    gpio_set_function(mosiPin, GPIO_FUNC_SPI);
    gpio_set_function(misoPin, GPIO_FUNC_SPI);
    gpio_deinit(csPin);
    gpio_deinit(sckPin);
    gpio_deinit(mosiPin);
    gpio_deinit(misoPin);
}

void spiHardwareAdapterEnable(uint8_t csPin) {
    gpio_put(csPin, 0);
}

void spiHardwareAdapterDisable(uint8_t csPin) {
    gpio_put(csPin, 1);
}

int spiHardwareAdapterReadBlocking(spi_inst_t *spi, uint8_t *dataRead, uint16_t lengthRead) {
    int numberOfBlocksRead = spi_read_blocking(spi, 0, dataRead, lengthRead);
    return numberOfBlocksRead;
}

int spiHardwareAdapterWriteBlocking(spi_inst_t *spi, uint8_t *dataToWrite, uint16_t lengthWrite) {
    int numberOfBlocksWritten = spi_write_blocking(spi, dataToWrite, lengthWrite);
    return numberOfBlocksWritten;
}
