#define SOURCE_FILE "GPIO-LIB"

#include "include/Gpio.h"
#include <hardware/gpio.h>

gpioErrorCode_t gpioSetPower(gpioPin_t pin, gpioPowerMode_t powerMode) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, powerMode);

    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioSetPinFunction(gpioPin_t pin, gpioPinFunction_t function) {
    gpio_set_function(pin, function);

    return GPIO_NO_ERROR;
}
