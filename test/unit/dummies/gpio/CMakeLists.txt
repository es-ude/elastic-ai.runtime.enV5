#[[add_elastic_ai_lib(
        NAME gpio_interface
        SRCS Gpio.c
        DEPS
            Common
            Gpio
)]]

add_elastic_ai_lib(
        NAME GpioDummy
        SRCS Gpio.c
        DEPS
            Gpio
)

