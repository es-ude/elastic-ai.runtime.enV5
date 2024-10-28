# Provides a Facade for the pico-sdk Time Library

add_elastic_ai_lib(
        HW_ONLY
        NAME Sleep
        SRCS ${CMAKE_CURRENT_LIST_DIR}/Sleep.c
        DEPS Common Pico::pico_time
)
