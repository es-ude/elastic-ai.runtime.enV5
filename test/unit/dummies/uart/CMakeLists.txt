add_library(uart_interface INTERFACE)
target_include_directories(uart_interface INTERFACE ${CMAKE_SOURCE_DIR}/src/hal/uart/include)
target_sources(uart_interface INTERFACE Uart.c)
target_link_libraries(uart_interface INTERFACE
        common_lib)
