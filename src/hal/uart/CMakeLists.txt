add_elastic_ai_lib(
        HW_ONLY
        NAME Uart
        SRCS Uart.c
        DEPS Common Gpio Pico::hardware_uart Pico::hardware_irq Pico::pico_runtime
)
