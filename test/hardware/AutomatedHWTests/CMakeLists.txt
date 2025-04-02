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


    add_executable(TestFileSystem TestFileSystem.c)
    target_link_libraries(TestFileSystem PRIVATE
            Common
            EnV5HwConfiguration
            EnV5HwController
            Esp
            FileSystem
            Flash
            FpgaConfigurationHandler
            Network
            Sleep

            Pico::pico_stdlib
            Pico::hardware_spi

            unity
    )

    create_enV5_executable(TestFileSystem)


    add_executable(CheckEspForOkViaPico CheckEspForOkViaPico.c)
    target_link_libraries(CheckEspForOkViaPico PRIVATE
            Pico::pico_stdlib
            Pico::hardware_uart
            Pico::hardware_gpio
            unity
    )
    create_enV5_executable(CheckEspForOkViaPico)
endif()