#ifndef ENV5_SPIHARDWAREADAPTER_H
#define ENV5_SPIHARDWAREADAPTER_H
#include <hardware/spi.h>

void spiHardwareAdapterInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin,
                            uint8_t mosiPin, uint8_t misoPin);

void spiHardwareAdapterDeinit(spi_inst_t *spi, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
                              uint8_t misoPin);

void spiHardwareAdapterEnable(uint8_t csPin);

void spiHardwareAdapterDisable(uint8_t csPin);

int spiHardwareAdapterReadBlocking(spi_inst_t *spi, uint8_t *dataRead, uint16_t lengthRead);

int spiHardwareAdapterWriteBlocking(spi_inst_t *spi, uint8_t *dataToWrite, uint16_t lengthWrite);
#endif // ENV5_SPIHARDWAREADAPTER_H
