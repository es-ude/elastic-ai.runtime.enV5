add_executable(hardware-test_configUSB HardwaretestConfigureViaUSB.c)
target_link_libraries(hardware-test_configUSB
        pico_stdlib
        hardware_spi
        spi_lib
        flash_lib
        sleep_interface
        enV5_hwc
        fpga_configuration_handler)
make_to_output_file(hardware-test_configUSB)

add_executable(hardware-test_configHTTP HardwaretestConfigureViaHTTP.c)
target_link_libraries(hardware-test_configHTTP
        pico_stdlib
        hardware_spi
        spi_lib
        flash_lib
        enV5_hwc
        fpga_configuration_handler)
make_to_output_file(hardware-test_configHTTP)
