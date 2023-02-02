#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER

#include <hardware/spi.h>

void spiInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
             uint8_t misoPin);

void spiDeinit(spi_inst_t *spi,  uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
               uint8_t misoPin);

void spiEnable(uint8_t csPin);

void spiDisable(uint8_t csPin);

int spiReadBlocking(spi_inst_t *spi, uint8_t *dataRead, uint16_t lengthRead);

int spiWriteBlocking(spi_inst_t *spi, uint8_t *dataToWrite, uint16_t lengthWrite);

#endif /* ENV5_SPI_HEADER */
