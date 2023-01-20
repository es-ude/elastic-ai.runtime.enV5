
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