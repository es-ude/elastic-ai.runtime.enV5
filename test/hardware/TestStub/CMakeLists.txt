## Library with stub for echo_server
add_library(echoserver_lib INTERFACE)
target_sources(echoserver_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/echo_server.c)
target_include_directories(echoserver_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(echoserver_lib INTERFACE middleware_lib)

## Test with HTTP Dummy Server
add_executable(hardware-test_stub HardwaretestStub.c)
target_link_libraries(hardware-test_stub
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        fpga_configuration_handler
        middleware_lib
        echoserver_lib)
target_compile_definitions(hardware-test_stub PRIVATE DEBUG)
make_to_output_file(hardware-test_stub)


## Test in combination with Monitor
add_executable(hardware-test_echoServerOTA HardwaretestStubWithMonitor.c)
target_link_libraries(hardware-test_echoServerOTA
        pico_stdlib
        hardware_spi
        flash_lib
        enV5_hwc
        fpga_configuration_handler
        middleware_lib
        espBroker_lib
        echoserver_lib)
target_compile_definitions(hardware-test_echoServerOTA PRIVATE DEBUG)
make_to_output_file(hardware-test_echoServerOTA)
