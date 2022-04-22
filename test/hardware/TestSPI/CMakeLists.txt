
add_executable(hardware-test_SPI test_spi.c)
target_link_libraries(hardware-test_SPI
        pico_stdlib
        FreeRTOSUtils
        hardware_spi
        spi_lib)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_SPI 1)
pico_enable_stdio_uart(hardware-test_SPI 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_SPI)
