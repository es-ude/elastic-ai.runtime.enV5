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
endif()