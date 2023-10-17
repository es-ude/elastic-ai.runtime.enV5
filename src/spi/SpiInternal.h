#ifndef ENV5_SPI_INTERNAL_HEADER
#define ENV5_SPI_INTERNAL_HEADER

#include <stdint.h>

static inline void spiDisableDevice(uint8_t chipSelectPin);
static inline void spiEnableDevice(uint8_t chipSelectPin);

#endif // ENV5_SPI_INTERNAL_HEADER
