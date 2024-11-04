# Provides a Facade for the pico-sdk GPIO library

add_elastic_ai_lib(
        HW_ONLY
        NAME Gpio
        SRCS Gpio.c
        DEPS
            Common
            Pico::hardware_gpio
)