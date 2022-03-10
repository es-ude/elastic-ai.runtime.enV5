#ifndef GPIO_LIBRARY_H
#define GPIO_LIBRARY_H

#include <stdint.h>

enum {
    GPIO_NO_ERROR   = 0x00,
    GPIO_INIT_ERROR = 0x10,
};
typedef uint16_t GPIO_ErrorCode;

typedef uint8_t GPIO_PIN;

enum {
    GPIO_POWER_UP   = 1,
    GPIO_POWER_DOWN = 0,
};
typedef uint8_t GPIO_POWER_MODE;

GPIO_ErrorCode GPIO_setPower(GPIO_PIN pin, GPIO_POWER_MODE powerMode);

#endif // GPIO_LIBRARY_H
