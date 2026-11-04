add_executable(ExampleUsbProtocol main.c)
target_link_libraries(ExampleUsbProtocol
        UsbProtocol
        EnV5HwConfiguration
        EnV5HwController
        Flash
        Gpio
        Sleep

        CException

        Pico::pico_stdlib
        Pico::hardware_spi
)
create_enV5_executable(ExampleUsbProtocol)
