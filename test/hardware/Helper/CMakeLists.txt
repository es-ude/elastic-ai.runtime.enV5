if(BUILDING_FOR_ELASTIC_NODE)
    add_library(HardwaretestHelper HardwaretestHelper.c)
    target_include_directories(HardwaretestHelper PUBLIC include)
    target_link_libraries(HardwaretestHelper PRIVATE
            Common
            Pico::pico_stdlib
            Pico::pico_bootrom
            MqttBroker
            Network
            freeRtosUtils
            EnV5HwController
            Esp
    )
else()
    add_library(HardwaretestHelper INTERFACE)
    target_include_directories(HardwaretestHelper INTERFACE include)
endif ()