#ifndef SENSOR_BOARD_UNITTESTS_GPIO_H
#define SENSOR_BOARD_UNITTESTS_GPIO_H

enum gpio_function {
    GPIO_FUNC_XIP = 0,
    GPIO_FUNC_SPI = 1,
    GPIO_FUNC_UART = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5,
    GPIO_FUNC_PIO0 = 6,
    GPIO_FUNC_PIO1 = 7,
    GPIO_FUNC_GPCK = 8,
    GPIO_FUNC_USB = 9,
    GPIO_FUNC_NULL = 0x1f,
};

void gpio_set_function(int gpio, enum gpio_function fn) {}

#endif /* SENSOR_BOARD_UNITTESTS_GPIO_H */
