/* standard SPI uses the Chip-Select pin negated
 * => 3,3V/HIGH -> Disable
 *      GND/LOW -> Enable
 */

/* IMPORTANT:
 *   spiEnableDevice has to be called before every function
 *   spiDisableDevice has to be called after every function
 *
 * Because the QX-SPI library currently uses the same SPI module with other setup!
 */

#define SOURCE_FILE "SPI"

#include <stdint.h>

#include "hardware/spi.h"

#include "Common.h"
#include "Gpio.h"
#include "Spi.h"
#include "SpiInternal.h"
#include "SpiTypedefs.h"

/* region PUBLIC HEADER FUNCTIONS */

void spiInit(spiConfig_t *spiConfiguration, uint8_t chipSelectPin) {
    uint32_t actualBaudrate = spi_init(spiConfiguration->spiInstance, spiConfiguration->baudrate);
    PRINT_DEBUG("Actual Baudrate: %lu", actualBaudrate);

    gpioSetPinFunction(spiConfiguration->sckPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(spiConfiguration->mosiPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(spiConfiguration->misoPin, GPIO_FUNCTION_SPI);
    gpioSetPinFunction(chipSelectPin, GPIO_FUNCTION_SPI);

    gpioInitPin(chipSelectPin, GPIO_OUTPUT);
    spiDisableDevice(chipSelectPin);
}
void spiDeinit(spiConfig_t *spiConfiguration, uint8_t chipSelectPin) {
    spi_deinit(spiConfiguration->spiInstance);

    gpioSetPinFunction(spiConfiguration->sckPin, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(spiConfiguration->mosiPin, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(spiConfiguration->misoPin, GPIO_FUNCTION_NULL);
    gpioSetPinFunction(chipSelectPin, GPIO_FUNCTION_NULL);

    gpioDisablePin(spiConfiguration->sckPin);
    gpioDisablePin(spiConfiguration->mosiPin);
    gpioDisablePin(spiConfiguration->misoPin);
    gpioDisablePin(chipSelectPin);
}

int spiWriteCommandBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command) {
    PRINT_BYTE_ARRAY_DEBUG("Command: ", command->data, command->length);

    spiEnableDevice(chipSelectPin);
    int numberOfBytesWritten =
        spi_write_blocking(spiConfiguration->spiInstance, command->data, command->length);
    spiDisableDevice(chipSelectPin);
    return numberOfBytesWritten;
}
int spiWriteCommandAndDataBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    PRINT_BYTE_ARRAY_DEBUG("Command: ", command->data, command->length);
    PRINT_BYTE_ARRAY_DEBUG("Data: ", data->data, data->length);

    spiEnableDevice(chipSelectPin);
    spi_write_blocking(spiConfiguration->spiInstance, command->data, command->length);
    int numberOfBytesWritten = spi_write_blocking(spiConfiguration->spiInstance, data->data, data->length);
    spiDisableDevice(chipSelectPin);
    return numberOfBytesWritten;
}
int spiWriteCommandAndReadBlocking(spiConfig_t *spiConfiguration, uint8_t chipSelectPin, data_t *command,
                                   data_t *data) {
    PRINT_BYTE_ARRAY_DEBUG("Command: ", command->data, command->length);

    spiEnableDevice(chipSelectPin);
    spi_write_blocking(spiConfiguration->spiInstance, command->data, command->length);
    int numberOfBlocksRead = spi_read_blocking(spiConfiguration->spiInstance, 0, data->data, data->length);
    spiDisableDevice(chipSelectPin);
    return numberOfBlocksRead;
}

/* endregion PUBLIC HEADER FUNCTIONS */

/* region INTERNAL HEADER FUNCTIONS */

static inline void spiEnableDevice(uint8_t chipSelectPin) {
    gpioSetPin(chipSelectPin, GPIO_PIN_LOW);
}

static inline void spiDisableDevice(uint8_t chipSelectPin) {
    gpioSetPin(chipSelectPin, GPIO_PIN_HIGH);
}

/* endregion INTERNAL HEADER FUNCTIONS */
