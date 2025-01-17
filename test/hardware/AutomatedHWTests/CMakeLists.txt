if(BUILDING_FOR_ELASTIC_NODE)
    add_executable(TestFlash TestFlash.c)
    target_link_libraries(TestFlash PRIVATE
            EnV5HwConfiguration
            Common
            Spi
            EnV5HwController
            Flash

            Pico::pico_stdlib
            Pico::hardware_spi

            unity
    )
    create_enV5_executable(TestFlash)
endif()