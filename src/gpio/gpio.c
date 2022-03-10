#include "gpio.h"
#include <hardware/gpio.h>

GPIO_ErrorCode GPIO_setPower(GPIO_PIN pin, GPIO_POWER_MODE powerMode) {
    gpio_init(29);
    gpio_set_dir(29, GPIO_OUT);
    gpio_put(29, powerMode);

    return GPIO_NO_ERROR;
}
