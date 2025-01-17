if(BUILDING_FOR_ELASTIC_NODE)
    add_executable(HardwareTestFlash HardwareTestFlash.c)
    target_link_libraries(HardwareTestFlash PRIVATE
            EnV5HwConfiguration
            Common
            Spi
            EnV5HwController
            Flash

            Pico::pico_stdlib
            Pico::hardware_spi
    )
    create_enV5_executable(HardwareTestFlash)

    add_executable(HardwareTestFlashReadConfiguration HardwareTestFlashReadConfiguration.c)
    target_link_libraries(HardwareTestFlashReadConfiguration
            Common
            Pico::pico_stdlib
            Pico::hardware_spi
            Spi
            EnV5HwConfiguration
            EnV5HwController
            Flash
            freeRtosUtils
    )
    create_enV5_executable(HardwareTestFlashReadConfiguration)
endif()