#ifndef ENV5_SPI_TYPEDEFS
#define ENV5_SPI_TYPEDEFS

#include <stddef.h>
#include <stdint.h>

typedef struct spi_inst spi_inst_t;

typedef struct data {
    uint8_t *data;
    size_t length;
} data_t;

typedef struct spiConfiguration {
    spi_inst_t *spiInstance;
    uint8_t sckPin;
    uint8_t misoPin;
    uint8_t mosiPin;
    uint32_t baudrate;
    uint8_t csPin;
} spiConfiguration_t;

#endif /* ENV5_SPI_TYPEDEFS */
