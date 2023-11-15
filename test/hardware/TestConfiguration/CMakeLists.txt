add_executable(hardware-test_configUSB HardwaretestConfigureViaUSB.c)
target_link_libraries(hardware-test_configUSB
        common_lib
        pico_stdlib
        hardware_spi
        spi_interface
        flash_lib
        sleep_interface
        enV5_hwc
        fpga_configuration_handler)
create_enV5_executable(hardware-test_configUSB)

add_executable(hardware-test_configHTTP HardwaretestConfigureViaHTTP.c)
target_link_libraries(hardware-test_configHTTP
        common_lib
        pico_stdlib
        hardware_spi
        spi_interface
        flash_lib
        enV5_hwc
        fpga_configuration_handler)
create_enV5_executable(hardware-test_configHTTP)
