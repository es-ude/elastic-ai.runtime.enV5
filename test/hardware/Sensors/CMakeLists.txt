##############################################
## ADD EXECUTABLE: hardware-test_sht3
add_executable(hardware-test_sht3 test_sht3x.c)
target_link_libraries(hardware-test_sht3
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_sht3x)
# enable usb output, disable uart output
pico_enable_stdio_usb(hardware-test_sht3 1)
pico_enable_stdio_uart(hardware-test_sht3 0)
# create map/bin/hex/uf2 file etc.
pico_add_uf2_output(hardware-test_sht3)
##############################################
