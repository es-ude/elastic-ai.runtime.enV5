add_elastic_ai_lib(
        HW_ONLY
        NAME Time
        SRCS Time.c
        DEPS Common Pico::hardware_timer Pico::pico_runtime
)