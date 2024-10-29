# Provides a Facade for the pico-sdk Time Library

add_elastic_ai_lib(
        HW_ONLY
        NAME Sleep
        SRCS Sleep.c
        DEPS Common Pico::pico_time
)
