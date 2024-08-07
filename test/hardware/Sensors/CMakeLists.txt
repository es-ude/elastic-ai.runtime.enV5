##############################################
## region hardware-test_sht3
add_executable(hardware-test_sht3 HardwaretestSht3x.c)
target_link_libraries(hardware-test_sht3
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        hardware_i2c

        common_lib
        enV5_hw_configuration
        enV5_hw_controller
        i2c_interface
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
        i2c_interface
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
        i2c_interface
        enV5_hw_configuration
        sensor_lib_pac193x)
create_enV5_executable(hardware-test_pac193x)
##############################################
## hardware-test_pac193x_stream
add_executable(hardware-test_pac193x_stream HardwaretestPac193xAccumulator.c)
target_link_libraries(hardware-test_pac193x_stream
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        i2c_interface
        enV5_hw_configuration
        enV5_hw_controller
        sensor_lib_pac193x)
create_enV5_executable(hardware-test_pac193x_stream)
##############################################
## hardware-test_dualpac193x
add_executable(hardware-test_dualpac193x HardwaretestDualPac193x.c)
target_link_libraries(hardware-test_dualpac193x
        common_lib
        pico_stdlib
        pico_stdio_usb
        pico_bootrom
        pico_time
        i2c_interface
        enV5_hw_configuration
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
        i2c_interface
        enV5_hw_configuration
        sensor_lib_pac193x)
create_enV5_executable(hardware-test_pac193xbuffer)
##############################################
