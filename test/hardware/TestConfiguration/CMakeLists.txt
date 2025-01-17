if(BUILDING_FOR_ELASTIC_NODE)
    add_executable(HardwareTestConfigureViaUSB HardwareTestConfigureViaUSB.c)
    target_link_libraries(HardwareTestConfigureViaUSB PRIVATE
            Common
            EnV5HwConfiguration
            EnV5HwController
            FpgaConfigurationHandler
            FpgaConfigurationHandlerUSB
            Flash
            Pico::pico_stdio
            Pico::pico_stdlib
            Pico::hardware_spi
    )
    create_enV5_executable(HardwareTestConfigureViaUSB)



    add_executable(HardwareTestConfigureViaHTTP HardwareTestConfigureViaHTTP.c)
    target_link_libraries(HardwareTestConfigureViaHTTP PRIVATE
            Common
            EnV5HwController
            EnV5HwConfiguration
            Esp
            Network
            Flash
            FpgaConfigurationHandler
            Pico::pico_stdlib
            Pico::pico_stdio_usb
            Pico::hardware_spi
    )
    create_enV5_executable(HardwareTestConfigureViaHTTP)
endif()