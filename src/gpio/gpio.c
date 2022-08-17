#include "gpio.h"

#include "hardware/gpio.h"

GPIO_ErrorCode GPIO_setPower(GPIO_PIN pin, GPIO_POWER_MODE powerMode) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, powerMode);

    return GPIO_NO_ERROR;
}

GPIO_ErrorCode GPIO_setPinFunction(GPIO_PIN pin, GPIO_PIN_FUNCTION function) {
    gpio_set_function(pin, function);

    return GPIO_NO_ERROR;
}
