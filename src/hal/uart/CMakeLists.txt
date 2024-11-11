if(BUILDING_FOR_ELASTIC_NODE)
    add_library(Uart Uart.c)
    target_include_directories(Uart PUBLIC include)
    target_link_libraries(Spi PRIVATE Common Gpio Pico::hardware_uart
            Pico::hardware_irq
            Pico::pico_runtime)
else()
    add_library(Uart INTERFACE)
    target_include_directories(Uart INTERFACE include)
endif ()
