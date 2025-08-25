#include "eai/hal2/SpiPico.h"
#include "hardware/spi.h"

#include <stdlib.h>

#include "eai/hal/Gpio.h"

typedef struct PicoSpi {
    Spi parent;
    spi_inst_t *spi;
    const SpiConfig *config;
} PicoSpi;

static PicoSpi *cast(Spi *self) {
    return (PicoSpi *)self;
}

static void setConfig(Spi *self, const SpiConfig *config) {
    PicoSpi *this = cast(self);
    this->config = config;
}

static inline uint8_t eai_to_pico_cpol(uint8_t cpol) {
    switch (cpol) {
    case EAI_SPI_CPOL_0:
        return SPI_CPOL_0;
    default:
        return SPI_CPOL_1;
    }
}

static inline uint8_t eai_to_pico_cpha(uint8_t cpha) {
    switch (cpha) {
    case EAI_SPI_CPHA_0:
        return SPI_CPHA_0;
    default:
        return SPI_CPHA_1;
    }
}

static inline uint8_t eai_to_pico_data_order(uint8_t data_order) {
    return SPI_MSB_FIRST;
}

static void init(Spi *self) {
    PicoSpi *this = cast(self);
    spi_init(this->spi, this->config->baudrate);
    uint8_t pico_cpol = eai_to_pico_cpol(this->config->polarity);
    uint8_t pico_cpha = eai_to_pico_cpha(this->config->phase);
    uint8_t pico_order = eai_to_pico_data_order(this->config->order);
    spi_set_format(this->spi, this->config->data_bits, pico_cpol, pico_cpha, pico_order);
    gpioSetPinFunction(this->config->sckPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(this->config->mosiPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(this->config->misoPin, GPIO_FUNCTION_SPI);
}

static void deinit(Spi *self) {
    PicoSpi *this = cast(self);
    spi_deinit(this->spi);

    gpioSetPinFunction(this->config->sckPin, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(this->config->mosiPin, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(this->config->misoPin, GPIO_FUNCTION_NULL);

    gpioDisablePin(this->config->sckPin);
    gpioDisablePin(this->config->mosiPin);
    gpioDisablePin(this->config->misoPin);
}

static void select(Spi *self, uint8_t slaveHandle) {
    gpioSetPin(slaveHandle, GPIO_PIN_LOW);
}

static void deselect(Spi *self, uint8_t slaveHandle) {
    gpioSetPin(slaveHandle, GPIO_PIN_HIGH);
}

static void setupSlave(Spi *self, uint8_t slaveHandle) {
    gpioSetPinFunction(slaveHandle, GPIO_FUNCTION_SPI);
    gpioInitPin(slaveHandle, GPIO_OUTPUT);
    deselect(self, slaveHandle);
}

static void write(Spi *self, const uint8_t *data, uint32_t length) {
    PicoSpi *this = cast(self);
    spi_write_blocking(this->spi, data, length);
}

static void read(Spi *self, uint8_t *data, uint32_t length) {
    PicoSpi *this = cast(self);
    spi_read_blocking(this->spi, 0, data, length);
}

static void destroy(Spi **self) {
    free(*self);
    *self = NULL;
}

Spi *PicoSpi_create(struct spi_inst *pico_instance) {
    static SpiFns fns = {
        .init = init,
        .deinit = deinit,
        .select = select,
        .deselect = deselect,
        .setupSlave = setupSlave,
        .write = write,
        .read = read,
        .destroy = destroy,
        .setConfig = setConfig,
    };
    PicoSpi *spi = malloc(sizeof(PicoSpi));
    spi->spi = pico_instance;
    spi->parent.fns = &fns;
    return (Spi *)spi;
}
