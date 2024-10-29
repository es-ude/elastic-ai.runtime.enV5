add_elastic_ai_lib(
        NAME Pac193x
        SRCS Pac193x.c
        DEPS
            Common
            Sleep
            Gpio
            I2c
)