#include "SpiHardwareAdapter.h"
#include "Gpio.h"
#include "SpiHardwareAdapterInternal.h"
#include <hardware/spi.h>
#include <stdint.h>

void spiInitChipSelect(const uint8_t csPin) {
    gpioEnablePin(csPin, GPIO_POWER_UP);
}

void spiHardwareAdapterInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin,
                            uint8_t mosiPin, uint8_t misoPin) {

    spiInitChipSelect(csPin);
    spi_init(spi, baudrate);

    gpioSetPinFunction(sckPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(mosiPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(misoPin, GPIO_FUNCTION_SPI);
}

void spiHardwareAdapterDeinit(spi_inst_t *spi, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
                              uint8_t misoPin) {
    spi_deinit(spi);

    gpioSetPinFunction(sckPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(mosiPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(misoPin, GPIO_FUNCTION_SPI);

    gpioDisablePin(csPin);
    gpioDisablePin(sckPin);
    gpioDisablePin(mosiPin);
    gpioDisablePin(misoPin);
}

void spiHardwareAdapterEnable(uint8_t csPin) {
    gpioSetPin(csPin, false);
}

void spiHardwareAdapterDisable(uint8_t csPin) {
    gpioSetPin(csPin, true);
}

int spiHardwareAdapterReadBlocking(spi_inst_t *spi, uint8_t *dataRead, uint16_t lengthRead) {
    int numberOfBlocksRead = spi_read_blocking(spi, 0, dataRead, lengthRead);
    return numberOfBlocksRead;
}

int spiHardwareAdapterWriteBlocking(spi_inst_t *spi, uint8_t *dataToWrite, uint16_t lengthWrite) {
    int numberOfBlocksWritten = spi_write_blocking(spi, dataToWrite, lengthWrite);
    return numberOfBlocksWritten;
}
