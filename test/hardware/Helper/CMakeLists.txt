if(BUILDING_FOR_ELASTIC_NODE)
    add_library(HardwareTestHelper HardwareTestHelper.c)
    target_include_directories(HardwareTestHelper PUBLIC include)
    target_link_libraries(HardwareTestHelper PRIVATE
            Pico::pico_stdlib
            Pico::pico_stdio_usb
            Pico::pico_bootrom

            Common
            MqttBroker
            Network
            freeRtosUtils
            EnV5HwController
            Esp
    )
else()
    add_library(HardwareTestHelper INTERFACE)
    target_include_directories(HardwareTestHelper INTERFACE include)
endif ()
