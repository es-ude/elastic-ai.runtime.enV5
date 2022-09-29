
add_executable(hardware-test_flash test_flash.c)
target_link_libraries(hardware-test_flash
        pico_stdlib
        FreeRTOSUtils
        hardware_spi
        spi_lib
        flash_lib)
target_compile_definitions(hardware-test_flash PRIVATE DEBUG)
make_to_output_file(hardware-test_flash)

# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_flash 1)
pico_enable_stdio_uart(hardware-test_flash 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_flash)
