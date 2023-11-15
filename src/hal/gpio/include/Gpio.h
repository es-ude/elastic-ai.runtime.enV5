#ifndef ENV5_GPIO_HEADER
#define ENV5_GPIO_HEADER

#include <stdbool.h>
#include <stdint.h>

typedef enum gpioErrorCode {
    GPIO_NO_ERROR = 0x00,
    GPIO_GENERAL_ERROR = 0x01,
    GPIO_INIT_ERROR = 0x10,
} gpioErrorCode_t;

typedef uint8_t gpioPin_t;

typedef enum gpioPinFunction {
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
} gpioPinFunction_t;

typedef enum gpioPinMode {
    GPIO_PIN_HIGH = 1,
    GPIO_PIN_LOW = 0,
} gpioPinMode_t;

typedef enum gpioDirection {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1,
} gpioDirection_t;

gpioErrorCode_t gpioInitPin(gpioPin_t pin, gpioDirection_t direction);

gpioErrorCode_t gpioDisablePin(gpioPin_t pin);

gpioErrorCode_t gpioSetPinFunction(gpioPin_t pin, gpioPinFunction_t function);

gpioErrorCode_t gpioEnablePullUp(gpioPin_t pin);

void gpioSetPin(gpioPin_t pin, gpioPinMode_t mode);

bool gpioGetPin(gpioPin_t pin);

#endif /* ENV5_GPIO_HEADER */
