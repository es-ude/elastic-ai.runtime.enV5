## Test with HTTP Dummy Server
add_executable(hardware-test_stub-v1 HardwaretestStub.c)
target_link_libraries(hardware-test_stub-v1
        common_lib
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        sleep_interface
        fpga_configuration_handler
        stub_v1_lib)
target_compile_definitions(hardware-test_stub-v1 PRIVATE DEBUG)
create_enV5_executable(hardware-test_stub-v1)

## Test with HTTP Dummy Server
add_executable(hardware-test_stub-v2 HardwaretestStub.c)
target_link_libraries(hardware-test_stub-v2
        common_lib
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        sleep_interface
        fpga_configuration_handler
        stub_v2_lib)
target_compile_definitions(hardware-test_stub-v2 PRIVATE DEBUG)
create_enV5_executable(hardware-test_stub-v2)

## Test in combination with Monitor
add_executable(hardware-test_echoServerOTA HardwaretestStubWithMonitor.c)
target_link_libraries(hardware-test_echoServerOTA
        common_lib
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        fpga_configuration_handler
        middleware_lib
        espBroker_lib
        stub_v1_lib)
target_compile_definitions(hardware-test_echoServerOTA PRIVATE DEBUG)
create_enV5_executable(hardware-test_echoServerOTA)
