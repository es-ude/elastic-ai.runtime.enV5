add_executable(hardware-test_middleware HardwaretestMiddleware.c)
target_link_libraries(hardware-test_middleware
        common_lib
        pico_stdlib
        hardware_spi
        network_lib
        esp_lib
        fpga_configuration_handler
        sleep_interface
        flash_lib
        middleware_v1_lib)
target_compile_definitions(hardware-test_middleware PRIVATE DEBUG)
create_enV5_executable(hardware-test_middleware)
