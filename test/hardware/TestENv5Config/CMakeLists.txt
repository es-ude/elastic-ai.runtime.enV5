
add_executable(hardware-test_fpga_config HardwaretestEnv5Config.c)
target_link_libraries(hardware-test_fpga_config
        pico_stdlib
        freeRtosUtils
        hardware_spi
        spi_lib
        flash_lib
        fpga_config_lib
        env5_hal
        middleware_lib)

target_compile_definitions(hardware-test_fpga_config PRIVATE DEBUG)
make_to_output_file(hardware-test_fpga_config)

# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_fpga_config 1)
pico_enable_stdio_uart(hardware-test_fpga_config 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_fpga_config)
