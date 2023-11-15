##############################################
## region hardware-test_sht3
add_executable(hardware-test_sht3 HardwaretestSht3x.c)
target_link_libraries(hardware-test_sht3
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        hardware_i2c
        sensor_lib_sht3x)
create_enV5_executable(hardware-test_sht3)
##############################################
## region hardware-test_adxl345b
add_executable(hardware-test_adxl345b HardwaretestAdxl345b.c)
target_link_libraries(hardware-test_adxl345b
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        hardware_i2c
        sensor_lib_adxl345b)
create_enV5_executable(hardware-test_adxl345b)
## endregion hardware-test_adxl345b
##############################################
## hardware-test_pac193x
add_executable(hardware-test_pac193x HardwaretestPac193x.c)
target_link_libraries(hardware-test_pac193x
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
create_enV5_executable(hardware-test_pac193x)
##############################################
## hardware-test_dualpac193x
add_executable(hardware-test_dualpac193x HardwaretestDualPac193x.c)
target_link_libraries(hardware-test_dualpac193x
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
create_enV5_executable(hardware-test_dualpac193x)
##############################################
## hardware-test_pac193xbuffer
add_executable(hardware-test_pac193xbuffer HardwaretestPac193xBuffer.c)
target_link_libraries(hardware-test_pac193xbuffer
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
create_enV5_executable(hardware-test_pac193xbuffer)
##############################################