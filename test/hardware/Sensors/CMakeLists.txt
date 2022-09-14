##############################################
## ADD EXECUTABLE: hardware-test_sht3
add_executable(hardware-test_sht3 test_sht3x.c)
target_link_libraries(hardware-test_sht3
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_sht3x)
make_to_output_file(hardware-test_sht3)
##############################################
## ADD EXECUTABLE: hardware-test_adxl345b
add_executable(hardware-test_adxl345b test_adxl345b.c)
target_link_libraries(hardware-test_adxl345b
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_adxl345b)
make_to_output_file(hardware-test_adxl345b)
##############################################
## ADD EXECUTABLE: hardware-test_pac193x
add_executable(hardware-test_pac193x test_pac193x.c)
target_link_libraries(hardware-test_pac193x
        pico_stdio
        pico_time
        hardware_i2c
        sensor_lib_pac193x)
make_to_output_file(hardware-test_pac193x)
##############################################