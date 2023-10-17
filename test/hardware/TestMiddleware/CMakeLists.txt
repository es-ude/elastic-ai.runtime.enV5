add_executable(hardware-test_middleware HardwaretestMiddleware.c)
target_link_libraries(hardware-test_middleware
        pico_stdlib
        hardware_spi
        middleware_lib)
target_compile_definitions(hardware-test_middleware PRIVATE DEBUG)
make_to_output_file(hardware-test_middleware)
