#ifndef ENV5_SPI_TYPEDEFS
#define ENV5_SPI_TYPEDEFS

#include <stddef.h>
#include <stdint.h>

typedef struct spi_inst spi_inst_t;

struct data {
    uint8_t *data;
    size_t length;
};
typedef struct data data_t;

struct spi {
    spi_inst_t *spi;
    uint8_t sckPin;
    uint8_t misoPin;
    uint8_t mosiPin;
    uint32_t baudrate;
};
typedef struct spi spi_t;

#endif /* ENV5_SPI_TYPEDEFS */
