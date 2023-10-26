
add_executable(hardware-test_flash HardwaretestFlash.c)
target_link_libraries(hardware-test_flash
        pico_stdlib
        freeRtosUtils
        hardware_spi
        spi_lib
        flash_lib)
make_to_output_file(hardware-test_flash)
