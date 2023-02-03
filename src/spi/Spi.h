#ifndef ENV5_SPI_HEADER
#define ENV5_SPI_HEADER
#include <stdint.h>
#include "FlashTypedefs.h"

void spiInit(spi_inst_t *spi, uint32_t baudrate, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
             uint8_t misoPin);

void spiDeinit(spi_inst_t *spi,  uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
               uint8_t misoPin);


int spiReadBlocking(spi_inst_t *spi,uint8_t csPin, uint8_t* cmd, uint8_t cmd_length, uint8_t
                                                                      *dataRead,
                    uint16_t lengthRead);

int spiWriteBlocking(spi_inst_t *spi, uint8_t csPin, uint8_t* cmd, uint8_t cmd_length,
                     uint8_t* data, uint16_t len);

void spiWriteSingleCmd(spi_inst_t *spi,uint8_t csPin, uint8_t* cmd,  uint8_t cmd_length);
#endif /* ENV5_SPI_HEADER */
