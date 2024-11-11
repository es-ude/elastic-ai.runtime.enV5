if(BUILDING_FOR_ELASTIC_NODE)
    add_library(UsbProtocol Protocol.c DefaultCommands.c Tools.c)
    target_include_directories(UsbProtocol PUBLIC include)
    target_link_libraries(UsbProtocol PRIVATE
            EnV5HwController
            EnV5HwConfiguration
            CException
            Pico::hardware_spi
            Gpio
            Flash
            Middleware
            Stub
            Sleep)
else()
    add_library(UsbProtocol INTERFACE)
    target_include_directories(UsbProtocol INTERFACE include)
endif ()