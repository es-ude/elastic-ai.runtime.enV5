if(BUILDING_FOR_ELASTIC_NODE)
    add_executable(HardwareTestFileSystem HardwareTestFileSystem.c)
    target_link_libraries(HardwareTestFileSystem PRIVATE
            FileSystem
            FpgaConfigurationHandler
            FpgaConfigurationHandlerUSB
            EnV5HwConfiguration
            Common
            Spi
            EnV5HwController
            Flash


            Esp
            Network

            Sleep



            Pico::pico_stdlib
            Pico::hardware_spi

    )
    create_enV5_executable(HardwareTestFileSystem)
endif()