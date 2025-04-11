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

    add_executable(TestFreeRTOSTask TestFreeRTOSTask.c)
    target_link_libraries(TestFreeRTOSTask PRIVATE
            Common
            EnV5HwConfiguration
            EnV5HwController
            Gpio
            freeRtosUtils

            freeRtos::FreeRTOS-Kernel
            Pico::hardware_watchdog
            Pico::pico_bootrom
            Pico::pico_stdlib

            unity
    )
    create_enV5_executable(TestFreeRTOSTask)

    add_executable(TestFreeRTOSTaskDualCore TestFreeRTOSTaskDualCore.c)
    target_link_libraries(TestFreeRTOSTaskDualCore PRIVATE
            Common
            EnV5HwConfiguration
            EnV5HwController
            Gpio
            freeRtosUtils

            freeRtos::FreeRTOS-Kernel
            Pico::hardware_watchdog
            Pico::pico_bootrom
            Pico::pico_stdlib

            unity
    )
    create_enV5_executable(TestFreeRTOSTaskDualCore)

    add_executable(TestFreeRTOSQueue TestFreeRTOSQueue.c)
    target_link_libraries(TestFreeRTOSQueue PRIVATE
            Common
            EnV5HwController
            freeRtosUtils

            freeRtos::FreeRTOS-Kernel
            Pico::pico_stdlib
            Pico::pico_bootrom
            Pico::hardware_spi

            unity
    )
    create_enV5_executable(TestFreeRTOSQueue)

    add_executable(TestFilesystem TestFileSystem.c)
    target_link_libraries(TestFilesystem PRIVATE
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
            Pico::pico_bootrom
            Pico::hardware_spi

            unity
    )
    create_enV5_executable(TestFilesystem)

    add_executable(CheckEspForOkViaPico CheckEspForOkViaPico.c)
    target_link_libraries(CheckEspForOkViaPico PRIVATE
            Pico::pico_stdlib
            Pico::hardware_uart
            Pico::hardware_gpio
            unity
    )
    create_enV5_executable(CheckEspForOkViaPico)
endif()