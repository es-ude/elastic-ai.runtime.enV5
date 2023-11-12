//
// Created by David P. Federl
//

#include "include/Gpio.h"

gpioErrorCode_t gpioInitPin(gpioPin_t pin, gpioDirection_t direction) {
    /* Just here to satisfy the compiler */
    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioDisablePin(gpioPin_t pin) {
    /* Just here to satisfy the compiler */
    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioSetPinFunction(gpioPin_t pin, gpioPinFunction_t function) {
    /* Just here to satisfy the compiler */
    return GPIO_NO_ERROR;
}

gpioErrorCode_t gpioEnablePullUp(gpioPin_t pin) {
    /* Just here to satisfy the compiler */
    return GPIO_NO_ERROR;
}

void gpioSetPin(gpioPin_t pin, gpioPinMode_t mode) {
    /* Just here to satisfy the compiler */
}

bool gpioGetPin(gpioPin_t pin) {
    return true;
}
