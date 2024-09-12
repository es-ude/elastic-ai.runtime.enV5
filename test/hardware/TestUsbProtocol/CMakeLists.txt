## Test USB-Protocol
add_executable(hardware-test_usb-protocol HardwaretestUsbProtocol.c)
target_link_libraries(hardware-test_usb-protocol
        pico_stdlib
        CException
        enV5_hw_controller
        usb-protocol
        common_lib
)
create_enV5_executable(hardware-test_usb-protocol)
