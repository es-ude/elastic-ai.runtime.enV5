#define SOURCE_FILE "SPI"

#include "include/Spi.h"
#include "include/SpiTypedefs.h"


void spiInit(spi_t *spiConfiguration, uint8_t chipSelectPin) {
    // Just here to satisfy the compiler
}
void spiDeinit(spi_t *spiConfiguration, uint8_t chipSelectPin) {
    // Just here to satisfy the compiler
}

int spiWriteCommandBlocking(spi_t *spiConfiguration, uint8_t chipSelectPin, data_t *command) {
    // Just here to satisfy the compiler
}
int spiWriteCommandAndDataBlocking(spi_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    // Just here to satisfy the compiler
}
int spiWriteCommandAndReadBlocking(spi_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    // Just here to satisfy the compiler
}
