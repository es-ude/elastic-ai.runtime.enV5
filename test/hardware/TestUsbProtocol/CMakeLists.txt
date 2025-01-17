## Test USB-Protocol
add_executable(HardwareTestUsbProtocol HardwareTestUsbProtocol.c)
target_link_libraries(HardwareTestUsbProtocol
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
create_enV5_executable(HardwareTestUsbProtocol)
