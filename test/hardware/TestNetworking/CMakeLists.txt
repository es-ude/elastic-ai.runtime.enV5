if(BUILDING_FOR_ELASTIC_NODE)

    add_library(NetworkTestCommon INTERFACE)
    target_link_libraries(NetworkTestCommon INTERFACE
            Common
            HardwareTestHelper
            MqttBroker
            freeRtosUtils
            RuntimeC::Protocol
            freeRtos::FreeRTOS-Kernel
            freeRtos::FreeRTOS-Kernel-Heap3
            Pico::pico_runtime
            Pico::pico_bootrom
    )


    add_executable(HardwareTestNetwork HardwareTestNetwork.c)
    target_link_libraries(HardwareTestNetwork PRIVATE
            NetworkTestCommon
            Network
    )
    target_compile_definitions(HardwareTestNetwork PRIVATE DEBUG)
    create_enV5_executable(HardwareTestNetwork)


    add_executable(HardwareTestCheckNetwork HardwareTestCheckNetwork.c)
    target_link_libraries(HardwareTestCheckNetwork PRIVATE
            NetworkTestCommon
            Network
    )
    target_compile_definitions(HardwareTestCheckNetwork PRIVATE DEBUG)
    create_enV5_executable(HardwareTestCheckNetwork)


    add_executable(HardwareTestMqttKeepAlive HardwareTestMqttKeepAlive.c)
    target_link_libraries(HardwareTestMqttKeepAlive PRIVATE
            NetworkTestCommon
            Sleep
    )
    target_compile_definitions(HardwareTestMqttKeepAlive PRIVATE DEBUG)
    create_enV5_executable(HardwareTestMqttKeepAlive)


    add_executable(HardwareTestMqttPublish HardwareTestMqttPublish.c)
    target_link_libraries(HardwareTestMqttPublish PRIVATE
            NetworkTestCommon
    )
    target_compile_definitions(HardwareTestMqttPublish PRIVATE DEBUG)
    create_enV5_executable(HardwareTestMqttPublish)


    add_executable(HardwareTestMqttSubscribe HardwareTestMqttSubscribe.c)
    target_link_libraries(HardwareTestMqttSubscribe PRIVATE
            Common
            HardwareTestHelper
            MqttBroker
            freeRtosUtils
            RuntimeC::Protocol
            freeRtos::FreeRTOS-Kernel
            freeRtos::FreeRTOS-Kernel-Heap3
            Pico::pico_runtime
            Pico::pico_bootrom
    )
    target_compile_definitions(HardwareTestMqttSubscribe PRIVATE DEBUG)
    create_enV5_executable(HardwareTestMqttSubscribe)


    add_executable(HardwareTestMqttSubscribeUnsubscribe HardwareTestMqttSubscribeUnsubscribe.c)
    target_link_libraries(HardwareTestMqttSubscribeUnsubscribe PRIVATE
            NetworkTestCommon
    )
    target_compile_definitions(HardwareTestMqttSubscribeUnsubscribe PRIVATE DEBUG)
    create_enV5_executable(HardwareTestMqttSubscribeUnsubscribe)


    add_executable(HardwareTestMqttPublishSubscribe HardwareTestMqttPublishSubscribe.c)
    target_link_libraries(HardwareTestMqttPublishSubscribe PRIVATE
            NetworkTestCommon
            Esp
    )
    target_compile_definitions(HardwareTestMqttPublishSubscribe PRIVATE DEBUG)
    create_enV5_executable(HardwareTestMqttPublishSubscribe)


    add_executable(HardwareTestDurability HardwareTestDurability.c)
    target_link_libraries(HardwareTestDurability PRIVATE
            NetworkTestCommon
            Pac193x
            Esp
            freeRtos::FreeRTOS-Kernel
            Pico::hardware_i2c
    )
    target_compile_definitions(HardwareTestDurability PRIVATE DEBUG)
    create_enV5_executable(HardwareTestDurability)


    add_executable(HardwareTestHTTP HardwareTestHTTP.c)
    target_link_libraries(HardwareTestHTTP PRIVATE
            NetworkTestCommon
            Network
            Http
            CException
    )
    target_compile_definitions(HardwareTestHTTP PRIVATE DEBUG)
    create_enV5_executable(HardwareTestHTTP)


    add_executable(HardwareTestPublishSensor HardwareTestPublishSensor.c)
    target_link_libraries(HardwareTestPublishSensor PRIVATE
            NetworkTestCommon
            Adxl345b
            Esp
            Pac193x
            Spi
            Network
            Http
            EnV5HwController

            Pico::hardware_i2c
            Pico::hardware_spi
            Pico::pico_stdlib
    )
    target_compile_definitions(HardwareTestPublishSensor PRIVATE DEBUG)
    create_enV5_executable(HardwareTestPublishSensor)

endif()