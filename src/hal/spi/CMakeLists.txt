# Provides a Facade for the pico-sdk SPI library

add_elastic_ai_lib(
        HW_ONLY
        NAME Spi
        SRCS Spi.c
        DEPS Common Gpio Pico::hardware_spi
)
