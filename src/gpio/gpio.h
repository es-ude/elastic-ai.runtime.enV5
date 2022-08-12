#ifndef GPIO_LIBRARY_H
#define GPIO_LIBRARY_H

#include <stdint.h>

enum {
    GPIO_NO_ERROR = 0x00,
    GPIO_GENERAL_ERROR = 0x01,
    GPIO_INIT_ERROR = 0x10,
};
typedef uint16_t GPIO_ErrorCode;

typedef uint8_t GPIO_PIN;

enum {
    GPIO_POWER_UP = 1,
    GPIO_POWER_DOWN = 0,
};
typedef uint8_t GPIO_POWER_MODE;

GPIO_ErrorCode GPIO_setPower(GPIO_PIN pin, GPIO_POWER_MODE powerMode);

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
typedef uint8_t GPIO_PIN_FUNCTION;

GPIO_ErrorCode GPIO_setPinFunction(GPIO_PIN pin, GPIO_PIN_FUNCTION function);

#endif // GPIO_LIBRARY_H
