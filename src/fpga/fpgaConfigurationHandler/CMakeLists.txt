add_library(
        FpgaConfigurationHandler
        FpgaConfigurationHandler.c
        FpgaConfigurationHandlerHTTP.c
)

target_include_directories(
        FpgaConfigurationHandler
        PUBLIC
        include
)


target_link_libraries(FpgaConfigurationHandler PRIVATE Common Http CException EnV5HwConfiguration Flash)


if(BUILDING_FOR_ELASTIC_NODE)
    add_library(FpgaConfigurationHandlerUSB FpgaConfigurationHandlerUSB.c)
    target_link_libraries(FpgaConfigurationHandlerUSB PRIVATE
            Common
            Http
            CException
            EnV5HwConfiguration
            Flash
            Pico::pico_stdio
            Pico::pico_stdlib
            Pico::pico_stdio_usb
            FpgaConfigurationHandler
    )
endif ()