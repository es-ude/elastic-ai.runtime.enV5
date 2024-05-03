#define SOURCE_FILE "SPI"

#include "Spi.h"
#include "SpiTypedefs.h"

void spiInit(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin) {
    // Just here to satisfy the compiler
}
void spiDeinit(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin) {
    // Just here to satisfy the compiler
}

int spiWriteCommandBlocking(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin, data_t *command) {
    // Just here to satisfy the compiler
}
int spiWriteCommandAndDataBlocking(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    // Just here to satisfy the compiler
}
int spiWriteCommandAndReadBlocking(spiConfiguration_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    // Just here to satisfy the compiler
}
