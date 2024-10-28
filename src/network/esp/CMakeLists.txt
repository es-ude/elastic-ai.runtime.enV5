add_elastic_ai_lib(
        HW_ONLY
        NAME Esp
        SRCS Esp.c
        DEPS Common Uart AtCommands EnV5HwController NetworkConfig Pico::hardware_uart freeRtosUtils
)