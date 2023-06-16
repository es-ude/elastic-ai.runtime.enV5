#ifndef ENV5_GPIO_HEADER
#define ENV5_GPIO_HEADER

#include <stdbool.h>
#include <stdint.h>

enum {
    GPIO_NO_ERROR = 0x00,
    GPIO_GENERAL_ERROR = 0x01,
    GPIO_INIT_ERROR = 0x10,
};
typedef uint16_t gpioErrorCode_t;

typedef uint8_t gpioPin_t;

enum {
    GPIO_POWER_UP = 1,
    GPIO_POWER_DOWN = 0,
};
typedef uint8_t gpioPowerMode_t;

enum {
    GPIO_FUNCTION_XIP = 0,
    GPIO_FUNCTION_SPI = 1,
    GPIO_FUNCTION_UART = 2,
    GPIO_FUNCTION_I2C = 3,
    GPIO_FUNCTION_PWM = 4,
    GPIO_FUNCTION_SIO = 5,
    GPIO_FUNCTION_PIO0 = 6,
    GPIO_FUNCTION_PIO1 = 7,
    GPIO_FUNCTION_GPCK = 8,
    GPIO_FUNCTION_USB = 9,
    GPIO_FUNCTION_NULL = 0x1f,
};
typedef uint8_t gpioPinFunction_t;

gpioErrorCode_t gpioEnablePin(gpioPin_t pin, gpioPowerMode_t powerMode);

gpioErrorCode_t gpioDisablePin(gpioPin_t pin);

gpioErrorCode_t gpioSetPinFunction(gpioPin_t pin, gpioPinFunction_t function);

gpioErrorCode_t gpioEnablePullUp(gpioPin_t pin);

gpioErrorCode_t gpioSetPin(gpioPin_t pin, bool setHigh);

#endif /* ENV5_GPIO_HEADER */
