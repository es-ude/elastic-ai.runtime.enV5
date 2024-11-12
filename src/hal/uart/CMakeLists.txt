if(BUILDING_FOR_ELASTIC_NODE)
    add_library(Uart Uart.c)
    target_include_directories(Uart PUBLIC include)
    target_link_libraries(Uart PRIVATE
            Common Gpio Pico::hardware_uart
            Pico::hardware_irq
            Pico::pico_runtime
            EnV5HwConfiguration
    )
else()
    add_library(Uart INTERFACE)
    target_include_directories(Uart INTERFACE include)
endif ()
