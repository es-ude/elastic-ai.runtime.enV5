##############################################
## region hardware-test_sht3
add_executable(hardware-test_sht3 HardwaretestSht3x.c)
target_link_libraries(hardware-test_sht3
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_sht3x)
make_to_output_file(hardware-test_sht3)
##############################################
## region hardware-test_adxl345b
add_executable(hardware-test_adxl345b HardwaretestAdxl345b.c)
target_link_libraries(hardware-test_adxl345b
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_adxl345b)
make_to_output_file(hardware-test_adxl345b)
## endregion hardware-test_adxl345b
##############################################
## hardware-test_pac193x
add_executable(hardware-test_pac193x HardwaretestPac193x.c)
target_link_libraries(hardware-test_pac193x
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
make_to_output_file(hardware-test_pac193x)
##############################################
## hardware-test_dualpac193x
add_executable(hardware-test_dualpac193x HardwaretestDualPac193x.c)
target_link_libraries(hardware-test_dualpac193x
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
make_to_output_file(hardware-test_dualpac193x)
##############################################
## hardware-test_pac193xbuffer
add_executable(hardware-test_pac193xbuffer HardwaretestPac193xBuffer.c)
target_link_libraries(hardware-test_pac193xbuffer
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
make_to_output_file(hardware-test_pac193xbuffer)
##############################################