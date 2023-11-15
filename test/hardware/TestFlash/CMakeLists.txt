
add_executable(hardware-test_flash HardwaretestFlash.c)
target_link_libraries(hardware-test_flash
        common_lib
        pico_stdlib
        freeRtosUtils
        hardware_spi
        spi_interface
        flash_lib)
make_to_output_file(hardware-test_flash)
