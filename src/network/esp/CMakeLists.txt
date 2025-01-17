if(BUILDING_FOR_ELASTIC_NODE)
    add_library(Esp Esp.c)
    target_include_directories(Esp PUBLIC include)
    target_link_libraries(Esp PRIVATE
            AtCommands
            Common
            EnV5HwConfiguration
            freeRtosUtils
            Uart
            Sleep
            Pico::pico_time
            Pico::hardware_uart
            Pico::pico_runtime


    )
else()
    add_library(Esp INTERFACE)
    target_include_directories(Esp INTERFACE include)
endif ()