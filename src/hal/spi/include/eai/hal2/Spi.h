#ifndef SPI_H
#define SPI_H
#include <stdint.h>

typedef struct Spi Spi;

typedef enum {
    EAI_SPI_CPOL_0 = 0,
    EAI_SPI_CPOL_1 = 1,
} SpiClockPolarity;

typedef enum {
    EAI_SPI_CPHA_0 = 0,
    EAI_SPI_CPHA_1 = 1,
} SpiClockPhase;

typedef enum {
    EAI_SPI_MSB_FIRST = 0,
    EAI_SPI_LSB_FIRST = 1,
} SpiOrder;

typedef struct SpiConfig {
    uint8_t sckPin;
    uint8_t misoPin;
    uint8_t mosiPin;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t data_bits;
    uint8_t order;
} SpiConfig;

typedef struct SpiFns {
    void (*init)(Spi *self);
    void (*deinit)(Spi *self);
    void (*setConfig)(Spi *self, const SpiConfig *config);
    void (*setupSlave)(Spi *self, uint8_t slaveHandle);
    void (*select)(Spi *self, uint8_t slaveHandle);
    void (*deselect)(Spi *self, uint8_t slaveHandle);
    void (*read)(Spi *self, uint8_t *data, uint32_t len);
    void (*write)(Spi *self, const uint8_t *data, uint32_t len);
    void (*destroy)(Spi **self);
} SpiFns;

struct Spi {
    const SpiFns *fns;
};

static inline void Spi_setConfig(Spi *self, const SpiConfig *config) {
    self->fns->setConfig(self, config);
}

static inline void Spi_init(Spi *self) {
    self->fns->init(self);
}

static inline void Spi_deinit(Spi *self) {
    self->fns->deinit(self);
}

static inline void Spi_read(Spi *self, uint8_t *data, uint32_t len) {
    self->fns->read(self, data, len);
}

static inline void Spi_write(Spi *self, const uint8_t *data, uint32_t len) {
    self->fns->write(self, data, len);
}

static inline void Spi_select(Spi *self, uint8_t slaveHandle) {
    self->fns->select(self, slaveHandle);
}

static inline void Spi_deselect(Spi *self, uint8_t slaveHandle) {
    self->fns->deselect(self, slaveHandle);
}

static inline void Spi_setupSlave(Spi *self, uint8_t slaveHandle) {
    self->fns->setupSlave(self, slaveHandle);
}
#endif // SPI_H
