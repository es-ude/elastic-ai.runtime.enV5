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

    add_executable(TestAdxl345b TestAdxl345b.c)
    target_link_libraries(TestAdxl345b PRIVATE
            EnV5HwConfiguration
            Common
            I2c
            EnV5HwController
            Adxl345b

            Pico::pico_stdlib
            Pico::hardware_i2c

            unity
    )
    create_enV5_executable(TestAdxl345b)
endif()