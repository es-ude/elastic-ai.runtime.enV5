#define SOURCE_FILE "GPIO-LIB"

#include "include/Gpio.h"
#include <hardware/gpio.h>

gpioErrorCode_t gpioInitPin(gpioPin_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);

    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioDisablePin(gpioPin_t pin) {
    gpio_deinit(pin);

    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioSetPinFunction(gpioPin_t pin, gpioPinFunction_t function) {
    gpio_set_function(pin, function);

    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioEnablePullUp(gpioPin_t pin) {
    gpio_pull_up(pin);

    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioSetPin(gpioPin_t pin, gpioPinMode_t mode) {
    gpio_put(pin, mode);
}