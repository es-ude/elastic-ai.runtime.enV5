#define SOURCE_FILE "SPI"

#include "Spi.h"
#include "SpiTypedefs.h"

void spiInit(spiConfig_t *spiConfiguration, uint8_t chipSelectPin) {
    // Just here to satisfy the compiler
}
void spiDeinit(spiConfig_t *spiConfiguration, uint8_t chipSelectPin) {
    // Just here to satisfy the compiler
}

int spiWriteCommandBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command) {
    // Just here to satisfy the compiler
}
int spiWriteCommandAndDataBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    // Just here to satisfy the compiler
}
int spiWriteCommandAndReadBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    // Just here to satisfy the compiler
}
