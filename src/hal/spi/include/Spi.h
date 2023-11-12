#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER

#include <stddef.h>
#include <stdint.h>

#include <SpiTypedefs.h>

void spiInit(spi_t *spiConfigration, uint8_t chipSelectPin);
void spiDeinit(spi_t *spiConfiguration, uint8_t chipSelectPin);

int spiWriteCommandBlocking(spi_t *spiConfiguration, uint8_t chipSelectPin, data_t *command);
int spiWriteCommandAndDataBlocking(spi_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data);
int spiWriteCommandAndReadBlocking(spi_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data);

#endif /* ENV5_SPI_HEADER */
