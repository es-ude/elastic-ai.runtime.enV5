add_elastic_ai_lib(
        HW_ONLY
        NAME Esp
        SRCS Esp.c
        DEPS AtCommands Common EnV5HwConfiguration freeRtosUtils Pico::hardware_uart Uart
)