
add_executable(hardware-test_middleware HardwaretestMiddleware.c)
target_link_libraries(hardware-test_middleware
        pico_stdlib
        hardware_spi
        middleware_lib)
target_compile_definitions(hardware-test_middleware PRIVATE DEBUG)
make_to_output_file(hardware-test_middleware)

# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_middleware 1)
pico_enable_stdio_uart(hardware-test_middleware 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_middleware)

# HardwareEchoServer stub test

add_library(echoserver_lib INTERFACE)
target_sources(echoserver_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/echo_server.c)
target_include_directories(echoserver_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(echoserver_lib INTERFACE middleware_lib)

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