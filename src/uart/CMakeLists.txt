add_library(uart_lib INTERFACE)
target_sources(uart_lib INTERFACE
        uartToESP.c)
target_include_directories(uart_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(uart_lib INTERFACE
        pico_stdlib
        hardware_gpio
        hardware_uart)
