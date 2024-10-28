add_elastic_ai_lib(
        NAME Adxl345b
        SRCS Adxl345b.c
        DEPS
            Common
            Sleep
            Time
            I2c
)