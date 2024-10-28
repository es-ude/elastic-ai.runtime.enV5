# Provides a Facade for the pico-sdk I2C library


add_elastic_ai_lib(
        HW_ONLY
        NAME I2c
        SRCS I2c.c
        DEPS Common Gpio Pico::hardware_i2c
)
