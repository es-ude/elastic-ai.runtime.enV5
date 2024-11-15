if(BUILDING_FOR_ELASTIC_NODE)
    add_executable(HardwareTestFileSystem HardwareTestFileSystem.c)
    target_link_libraries(HardwareTestFileSystem PRIVATE
            FileSystem
            FpgaConfigurationHandler
            Common
            Spi
            Flash
            Sleep
            Esp
            Network
            EnV5HwConfiguration
            EnV5HwController

            Pico::pico_stdlib
            Pico::hardware_spi

    )
    create_enV5_executable(HardwareTestFileSystem)
endif()