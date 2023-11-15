add_library(uart_interface INTERFACE)
target_include_directories(uart_interface INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(uart_interface INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Uart.c)
target_link_libraries(uart_interface INTERFACE
        common_lib
        gpio_interface
        hardware_uart
        hardware_irq)
