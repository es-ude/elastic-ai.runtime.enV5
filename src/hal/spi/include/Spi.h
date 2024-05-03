#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER

#include <stddef.h>
#include <stdint.h>

#include <SpiTypedefs.h>

void spiInit(spiConfiguration_t *spiConfiguration);
void spiDeinit(spiConfiguration_t *spiConfiguration);

int spiWriteCommandBlocking(spiConfiguration_t *spiConfiguration, data_t *command);
int spiWriteCommandAndDataBlocking(spiConfiguration_t *spiConfiguration, data_t *command,
                                   data_t *data);
int spiWriteCommandAndReadBlocking(spiConfiguration_t *spiConfiguration, data_t *command,
                                   data_t *data);

#endif /* ENV5_SPI_HEADER */
