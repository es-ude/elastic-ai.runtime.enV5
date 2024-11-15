if(BUILDING_FOR_ELASTIC_NODE)
    add_library(HardwareTestCommon INTERFACE)
    target_link_libraries(HardwareTestCommon INTERFACE
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

    add_library(HardwareTestNetwork INTERFACE)
    target_link_libraries(HardwareTestNetwork INTERFACE
            Esp
            NetworkConfig
            MqttBroker
            HardwareTestHelper
            #RuntimeC::Protocol
            Network
            freeRtosUtils
    )

    add_executable(HardwareTestSht3x HardwareTestSht3x.c)
    target_link_libraries(HardwareTestSht3x PRIVATE
            Sht3x
            HardwareTestCommon
    )
    create_env5_executable(HardwareTestSht3x)


    add_executable(HardwareTestAdxl345b HardwareTestAdxl345b.c)
    target_link_libraries(HardwareTestAdxl345b PRIVATE
            Adxl345b
            HardwareTestCommon
    )
    create_enV5_executable(HardwareTestAdxl345b)


    add_executable(HardwareTestPac193x HardwareTestPac193x.c)
    target_link_libraries(HardwareTestPac193x PRIVATE
            Pac193x
            HardwareTestCommon
    )
    create_enV5_executable(HardwareTestPac193x)


    add_executable(HardwareTestPac193xAccumulator HardwareTestPac193xAccumulator.c)
    target_link_libraries(HardwareTestPac193xAccumulator PRIVATE
            Pac193x
            HardwareTestCommon
    )
    create_enV5_executable(HardwareTestPac193xAccumulator)


    add_executable(HardwareTestDualPac193x HardwareTestDualPac193x.c)
    target_link_libraries(HardwareTestDualPac193x PRIVATE
            Pac193x
            HardwareTestCommon
    )
    create_enV5_executable(HardwareTestDualPac193x)


    add_executable(HardwareTestPac193xBuffer HardwareTestPac193xBuffer.c)
    target_link_libraries(HardwareTestPac193xBuffer PRIVATE
            Pac193x
            HardwareTestCommon
    )
    create_enV5_executable(HardwareTestPac193xBuffer)



    add_executable(HardwareTestBmi323 HardwareTestBmi323.c)
    target_link_libraries(HardwareTestBmi323 PRIVATE
            Bmi323
            HardwareTestCommon
    )
    create_enV5_executable(HardwareTestBmi323)


    add_executable(StressTestAdxl345b StressTestAdxl345b.c)
    target_link_libraries(StressTestAdxl345b PRIVATE
            Adxl345b
            HardwareTestCommon
            HardwareTestNetwork
    )
    pico_enable_stdio_usb(Network 1)
    # disable uart output
    pico_enable_stdio_uart(Network 0)
    create_enV5_executable(StressTestAdxl345b)
endif ()

