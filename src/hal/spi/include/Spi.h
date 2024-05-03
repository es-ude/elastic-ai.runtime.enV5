#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER

#include <stddef.h>
#include <stdint.h>

#include <SpiTypedefs.h>

void spiInit(spiConfiguration_t *spiConfigration, uint8_t chipSelectPin);
void spiDeinit(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin);

int spiWriteCommandBlocking(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin, data_t *command);
int spiWriteCommandAndDataBlocking(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data);
int spiWriteCommandAndReadBlocking(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data);

#endif /* ENV5_SPI_HEADER */
