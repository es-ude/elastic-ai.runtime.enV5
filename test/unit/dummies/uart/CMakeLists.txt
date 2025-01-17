add_library(UartDummy Uart.c)
target_link_libraries(UartDummy PUBLIC Uart)
target_link_libraries(UartDummy PRIVATE Common)