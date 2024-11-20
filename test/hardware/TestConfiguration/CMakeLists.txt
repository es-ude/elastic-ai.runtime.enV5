if(BUILDING_FOR_ELASTIC_NODE)
    add_library(ConfigTestCommon INTERFACE)
    target_link_libraries(ConfigTestCommon INTERFACE
            I2c
            Gpio
            Sleep
            Spi
            Pico::pico_stdlib
            Pico::pico_stdio
            Pico::pico_stdio_usb
            Pico::pico_time
            Pico::pico_runtime
            Pico::pico_bootrom
            Pico::hardware_i2c
            Pico::hardware_spi
            EnV5HwController
            Common
            EnV5HwConfiguration
            CException
    )

    add_library(ConfigTestNetwork INTERFACE)
    target_link_libraries(ConfigTestNetwork INTERFACE
            Esp
            Network
            NetworkConfig

            freeRtosUtils
            RuntimeC::Protocol
            RuntimeC::TopicMatcher
            freeRtos::FreeRTOS-Kernel
            freeRtos::FreeRTOS-Kernel-Heap3
    )

    add_executable(HardwareTestConfigureViaUSB HardwareTestConfigureViaUSB.c)
    target_link_libraries(HardwareTestConfigureViaUSB PRIVATE

            FpgaConfigurationHandler
            FpgaConfigurationHandlerUSB
            ConfigTestCommon
            Flash
    )
    create_enV5_executable(HardwareTestConfigureViaUSB)

    add_executable(HardwareTestConfigureViaHTTP HardwareTestConfigureViaHTTP.c)
    target_link_libraries(HardwareTestConfigureViaHTTP PRIVATE
            ConfigTestCommon
            ConfigTestNetwork
            Flash
            FpgaConfigurationHandler)
    create_enV5_executable(HardwareTestConfigureViaHTTP)
endif()