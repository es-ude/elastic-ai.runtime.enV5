#include "Spi.h"
#include "SpiInternal.h"
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <stdint.h>

void spiInitChipSelect(const uint8_t csPin) {
    gpio_init(csPin);
    gpio_set_dir(csPin, GPIO_OUT);
    gpio_put(csPin, 1);
}

void spiInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
             uint8_t misoPin) {

    spiInitChipSelect(csPin);
    spi_init(spi, baudrate);

    gpio_set_function(sckPin, GPIO_FUNC_SPI);
    gpio_set_function(mosiPin, GPIO_FUNC_SPI);
    gpio_set_function(misoPin, GPIO_FUNC_SPI);
}

void spiDeinit(spi_inst_t *spi, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
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

void spiEnable(uint8_t csPin) {
    gpio_put(csPin, 0);
}

void spiDisable(uint8_t csPin) {
    gpio_put(csPin, 1);
}

int spiReadBlocking(spi_inst_t *spi, uint8_t *dataRead, uint16_t lengthRead) {
    int numberOfBlocksRead = spi_read_blocking(spi, 0, dataRead, lengthRead);
    return numberOfBlocksRead;
}

int spiWriteBlocking(spi_inst_t *spi, uint8_t *dataToWrite, uint16_t lengthWrite) {
    int numberOfBlocksWritten = spi_write_blocking(spi, dataToWrite, lengthWrite);
    return numberOfBlocksWritten;
}
