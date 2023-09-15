add_library(uart_lib INTERFACE)
target_sources(uart_lib INTERFACE Uart.c)
target_include_directories(uart_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(uart_lib INTERFACE
        gpio_interface
        hardware_uart
        hardware_irq)
