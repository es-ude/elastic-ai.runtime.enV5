
add_executable(hardware-test_config test_configuration.c)
target_link_libraries(hardware-test_config
        pico_stdlib
        FreeRTOSUtils
        hardware_spi
        spi_lib
        flash_lib
        fpga_config_lib)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_config 1)
pico_enable_stdio_uart(hardware-test_config 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_config)
