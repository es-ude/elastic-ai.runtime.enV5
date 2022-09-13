
add_executable(hardware-test_fpga_config test_env5_config.c)
add_library(env5 env5_hw.c)
target_link_libraries(hardware-test_fpga_config
        pico_stdlib
        FreeRTOSUtils
        hardware_spi
        spi_lib
        flash_lib
        fpga_config_lib
        env5)
target_link_libraries(env5
        pico_stdlib)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_fpga_config 1)
pico_enable_stdio_uart(hardware-test_fpga_config 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_fpga_config)
