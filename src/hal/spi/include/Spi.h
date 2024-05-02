#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER

#include <stddef.h>
#include <stdint.h>

#include <SpiTypedefs.h>

void spiInit(spiConfig_t *spiConfigration, uint8_t chipSelectPin);
void spiDeinit(spiConfig_t *spiConfiguration, uint8_t chipSelectPin);

int spiWriteCommandBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command);
int spiWriteCommandAndDataBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data);
int spiWriteCommandAndReadBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data);

#endif /* ENV5_SPI_HEADER */
